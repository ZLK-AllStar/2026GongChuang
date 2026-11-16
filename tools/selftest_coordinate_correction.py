"""Check coordinate correction in host scripts without camera, model or UART."""
import importlib.util
import math
import sys
from pathlib import Path
from unittest.mock import patch


root = Path(__file__).resolve().parents[1]
scripts = [Path(arg) for arg in sys.argv[1:]] or [
    root / '上位机脚本/cv2_copy.py',
    root / '上位机脚本/_cv2 copy.py',
    root / 'classic-new/_cv2.py',
]
checks = 0


def check(name, condition):
    global checks
    assert condition, name
    checks += 1


def close(actual, expected):
    return actual is not None and all(
        math.isclose(a, b, abs_tol=1e-6) for a, b in zip(actual, expected)
    )


for index, script in enumerate(scripts):
    spec = importlib.util.spec_from_file_location(f'correction_under_test_{index}', script)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    identity = module.np.eye(3, dtype=module.np.float64)
    names = ('MACHINE_X_SCALE', 'MACHINE_Y_SCALE', 'MACHINE_X_OFFSET_MM', 'MACHINE_Y_OFFSET_MM')
    defaults = tuple(getattr(module, name) for name in names)
    check('deployed correction defaults are finite',
          all(math.isfinite(value) for value in defaults))
    x_scale, y_scale, x_offset, y_offset = defaults
    for point in [(0, 0), (100, 100), (215, 205), (216, 206), (-1, -1)]:
        check('default conversion applies configured scale/offset and Y clamp',
              close(module.pixel_to_machine(point, identity),
                    (point[0] * x_scale + x_offset,
                     min(point[1] * y_scale + y_offset, 205))))
    with patch.multiple(module, MACHINE_X_SCALE=1.1, MACHINE_Y_SCALE=0.9,
                        MACHINE_X_OFFSET_MM=2.0, MACHINE_Y_OFFSET_MM=-3.0):
        corrected = module.pixel_to_machine((100, 100), identity)
        check('independent axis scale and signed offset', close(corrected, (112, 87)))
        h = identity.copy()
        h[0] = (2, 0, 5)
        h[1] = (0, 3, 7)
        check('correction follows the calibrated homography',
              close(module.pixel_to_machine((10, 20), h), (29.5, 57.3)))
        packet = module.build_pick_packet(42, 46, *corrected)
        check('encoder receives corrected position exactly once',
              int.from_bytes(packet[5:7], 'little') == 112
              and int.from_bytes(packet[7:9], 'little') == 87
              and packet[-1] == sum(packet[:-1]) & 255)
    with patch.object(module, 'MACHINE_Y_OFFSET_MM', -10.0):
        check('negative correction precedes upper Y clamp',
              close(module.pixel_to_machine((100, 206), identity),
                    (100 * x_scale + x_offset, 196)))
        check('negative Y stays invalid for the caller',
              close(module.pixel_to_machine((100, 5), identity),
                    (100 * x_scale + x_offset, -5)))
    with patch.object(module, 'MACHINE_Y_OFFSET_MM', 10.0):
        check('positive correction respects Y 205 limit',
              close(module.pixel_to_machine((100, 200), identity),
                    (100 * x_scale + x_offset, 205)))
    with patch.object(module, 'MACHINE_Y_SCALE', 0.5):
        check('Y scale precedes upper clamp',
              close(module.pixel_to_machine((100, 300), identity),
                    (100 * x_scale + x_offset, 300 * 0.5 + y_offset)))
    with patch.object(module, 'MACHINE_X_OFFSET_MM', 10.0):
        check('out of range X remains visible to workspace validation',
              close(module.pixel_to_machine((210, 100), identity),
                    (220, 100 * y_scale + y_offset)))
    check('missing calibration still prevents coordinates', module.pixel_to_machine((100, 100), None) is None)
    for name in names:
        for invalid in (math.nan, math.inf, -math.inf):
            with patch.object(module, name, invalid):
                check('nonfinite correction cannot create a sendable position',
                      module.pixel_to_machine((100, 100), identity) is None)
    for point in [(math.nan, 100), (100, math.inf), (-math.inf, 100)]:
        check('nonfinite input is rejected before limiting', module.correct_machine_coordinates(*point) is None)
    with patch.object(module, 'MACHINE_X_SCALE', 2.0):
        check('arithmetic overflow is rejected', module.correct_machine_coordinates(1e308, 100) is None)
    print(f'PASS: {script.name}')

print(f'PASS: {checks} coordinate correction checks; no physical motion')
