"""Four-point ROI regression without a camera, model weights, GUI or UART."""
import importlib.util
import json
import sys
import tempfile
from contextlib import ExitStack
from pathlib import Path
from unittest.mock import patch


checks = 0


def check(name, condition):
    global checks
    assert condition, name
    checks += 1


def load_module(path, index):
    spec = importlib.util.spec_from_file_location(f"roi_under_test_{index}", path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def select(module, actions, frame, scale=0.5):
    callback = None
    services = []
    canvases = []
    closed = False
    actions = iter(actions)

    def set_callback(window, function):
        nonlocal callback
        callback = function

    def wait_key(delay):
        nonlocal closed
        action = next(actions)  # Exhaustion detects a selector that never returns.
        if action[0] == "key":
            return action[1]
        if action[0] == "close":
            closed = True
        else:
            event = module.cv2.EVENT_LBUTTONDOWN if action[0] == "click" else module.cv2.EVENT_RBUTTONDOWN
            callback(event, action[1], action[2], 0, None)
        return -1

    def visible(*args):
        if closed:
            raise module.cv2.error("window closed")
        return 1

    def destroy(*args):
        if closed:
            raise module.cv2.error("already closed")

    with ExitStack() as stack:
        replacements = {
            "namedWindow": lambda *args: None,
            "setMouseCallback": set_callback,
            "imshow": lambda window, image: canvases.append(image.copy()),
            "waitKey": wait_key,
            "getWindowProperty": visible,
            "destroyWindow": destroy,
        }
        for name, value in replacements.items():
            stack.enter_context(patch.object(module.cv2, name, value))
        result = module.select_quad_roi(frame, scale, lambda: services.append(True))
    check("serial service remains live during selection", len(services) == len(canvases) > 0)
    return result, canvases


def test_module(module):
    np, cv2 = module.np, module.cv2
    shape = (100, 140, 3)
    quad = ((30, 10), (110, 10), (130, 90), (10, 90))
    check("clockwise quad accepted", module.validate_quad_roi(quad, shape) == quad)
    check("counterclockwise quad accepted", module.validate_quad_roi(quad[::-1], shape) == quad[::-1])
    check("inclusive polygon bounding crop", module.roi_bounds(quad) == (10, 10, 121, 81))
    check("legacy crop unchanged", module.roi_bounds((10, 10, 120, 80)) == (10, 10, 120, 80))
    check("polygon corner and center inside", module.roi_contains(quad, (30, 10)) and module.roi_contains(quad, (70, 50)))
    check("bounding corner outside polygon", not module.roi_contains(quad, (15, 15)))
    mask = module.roi_mask(quad)
    check("mask rejects bounding corner", mask[5, 5] == 0)
    check("mask retains polygon interior", mask[40, 60] == 255)
    check("legacy rectangles need no mask", module.roi_mask((0, 0, 140, 100)) is None)

    invalid = [(), quad[:3], (quad[0], quad[2], quad[1], quad[3]),
               (quad[0], quad[1], quad[1], quad[3]),
               ((10, 10), (30, 10), (50, 10), (70, 10)),
               ((1, 1), (3, 1), (3, 3), (1, 3)),
               ((-1, 10), *quad[1:]), ((140, 10), *quad[1:]),
               ((float("nan"), 10), *quad[1:]), ((float("inf"), 10), *quad[1:])]
    for points in invalid:
        try:
            module.validate_quad_roi(points, shape)
        except ValueError:
            check("invalid quad rejected", True)
        else:
            check("invalid quad rejected", False)

    with tempfile.TemporaryDirectory(prefix="huatai-roi-") as directory:
        path = Path(directory) / "roi.json"
        check("missing file allowed", module.load_roi_config(shape, path) is None)
        module.save_roi_config(quad, shape, path)
        payload = json.loads(path.read_text())
        check("polygon stored as version 3", payload["version"] == 3 and payload["shape"] == "quadrilateral")
        check("four original pixel points stored", payload["points_pixel"] == [list(point) for point in quad])
        check("quad round trip", module.load_roi_config(shape, path) == quad)
        check("resolution change scales all points", module.load_roi_config((200, 280, 3), path) == tuple((x * 2, y * 2) for x, y in quad))
        check("atomic save leaves no temporary file", not path.with_suffix(".json.tmp").exists())
        module.save_roi_config((10, 12, 110, 75), shape, path)
        legacy = json.loads(path.read_text())
        check("legacy rectangle round trip", module.load_roi_config(shape, path) == (10, 12, 110, 75) and legacy["version"] == 2)
        module.save_roi_config((10, 10, 2, 2), shape, path)
        check("tiny legacy ROI rejected", module.load_roi_config(shape, path) is None)
        for bad in (None, [], {"version": 3}, {**legacy, "version": 3, "points_normalized": []},
                    {**legacy, "version": 3, "points_normalized": [[0, 0], [1, 1], [1, 0], [0, 1]]},
                    {"version": 3, "points_normalized": [[0, 0], [2, 0], [1, 1], [0, 1]]},
                    {"version": 3, "points_normalized": [[float("nan"), 0]] * 4}):
            path.write_text(json.dumps(bad), encoding="utf-8")
            check("bad polygon never falls back to bounding rectangle", module.load_roi_config(shape, path) is None)
        path.write_text("{broken", encoding="utf-8")
        check("broken JSON rejected", module.load_roi_config(shape, path) is None)

    frame = np.full(shape, (0, 0, 255), np.uint8)
    clicks = [("click", x // 2, y // 2) for x, y in quad]
    result, _ = select(module, [("key", 13), *clicks, ("key", 13)], frame)
    check("four mouse points map from display to camera pixels", result == quad)
    result, _ = select(module, [*clicks, ("right", 0, 0), clicks[-1], ("key", 32)], frame)
    check("right click undoes a vertex and space confirms", result == quad)
    result, _ = select(module, [*clicks, ("key", 8), clicks[-1], ("key", 13)], frame)
    check("backspace undoes a vertex", result == quad)
    result, _ = select(module, [clicks[0], ("key", ord("r")), *clicks, ("key", 13)], frame)
    check("R clears the selection", result == quad)
    result, _ = select(module, [*clicks, ("key", 27)], frame)
    check("Esc discards an unconfirmed quad", result is None)
    result, _ = select(module, [("click", 10, 10), ("close",)], frame)
    check("window close cancels cleanly", result is None)
    result, _ = select(module, [clicks[0], clicks[2], clicks[1], clicks[3], ("key", 13), ("key", 27)], frame)
    check("crossed edges cannot be confirmed", result is None)
    odd_frame = np.zeros((101, 141, 3), np.uint8)
    result, _ = select(module, [("click", 10, 10), ("click", 40, 10), ("click", 40, 40), ("click", 10, 40), ("key", 13)], odd_frame)
    expected = tuple((int(round(x * 141 / 70)), int(round(y * 101 / 50))) for x, y in ((10, 10), (40, 10), (40, 40), (10, 40)))
    check("odd image dimensions use the actual display scale", result == expected)

    identity = np.eye(3, dtype=np.float64)
    if hasattr(module, "YoloShapeDetector"):
        class Detector:
            names = {0: "cuboid"}

            def predict(self, image):
                check("model input stays at original crop resolution", image.shape == (81, 121, 3))
                check("model sees black outside polygon", not image[5, 5].any())
                check("model retains original interior color", tuple(image[40, 60]) == (0, 0, 255))
                # Fake model returns one object outside and one inside the quad.
                return [[0, 0, 10, 10, .99, 0], [40, 20, 80, 60, .95, 0]]

        for scale in (1.0, .5, .37):
            _, masks, found = module.process_frame(frame, quad, 1, .95, 0, scale,
                                                   identity, False, Detector())
            check("outside model prediction is excluded", len(found) == 1)
            check("original pixel and machine coordinates are preserved", found[0].center == (70, 50) and found[0].machine_center == (70., 50.))
            check("inside target retains color and protocol mapping", found[0].color == "red" and found[0].command == module.encode_hmi_command("cuboid", "red"))
            scaled_mask = cv2.resize(mask, next(iter(masks.values())).shape[::-1], interpolation=cv2.INTER_NEAREST)
            check("every color mask excludes the polygon exterior", all(not image[scaled_mask == 0].any() for image in masks.values()))
        packet = module.build_pick_packet(1, found[0].command, *found[0].machine_center)
        check("UART carries unchanged calibrated center", int.from_bytes(packet[5:7], "little") == 70 and int.from_bytes(packet[7:9], "little") == 50)
    else:
        scene = np.zeros(shape, np.uint8)
        cv2.rectangle(scene, (11, 11), (22, 22), (0, 0, 255), -1)
        cv2.rectangle(scene, (50, 30), (90, 70), (0, 0, 255), -1)
        _, masks, found = module.process_frame(scene, quad, 5, .95, 0, 1.0, identity, False)
        check("contour pipeline excludes objects outside quad", len(found) == 1 and found[0].center == (70, 50))
        check("contour center remains in original coordinates", found[0].machine_center == (70., 50.))
        check("contour masks exclude polygon exterior", all(not image[mask == 0].any() for image in masks.values()))


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    scripts = [Path(arg) for arg in sys.argv[1:]] or [root / "上位机脚本/cv2_copy.py",
               root / "上位机脚本/_cv2 copy.py", root / "classic-new/_cv2.py"]
    for index, script in enumerate(scripts):
        test_module(load_module(script, index))
        print(f"PASS ROI: {script}")
    print(f"PASS: {checks} ROI checks; fake detector and GUI only, no camera/UART/motion")
