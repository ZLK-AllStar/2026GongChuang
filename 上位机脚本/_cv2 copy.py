"""YOLO 彩色图形状检测 + HSV 五色识别 + 像素/机械坐标显示。

默认加载 train13 的 best.pt，只预览，不打开串口。
原抓取状态机与协议保留；圆锥、白色暂不分配抓取指令。
"""

import argparse
import json
import math
import struct
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, List, Optional, Sequence, Tuple, Union

import cv2
import numpy as np


WINDOW_RESULT = "YOLO shapes + HSV colors"
WINDOW_MASKS = "Color masks"
PROJECT_DIR = Path(__file__).resolve().parent.parent
CONFIG_DIR = Path(__file__).resolve().parent / "config"
DEFAULT_ROI_FILE = CONFIG_DIR / "roi.json"
DEFAULT_CALIBRATION_FILE = CONFIG_DIR / "calibration.json"
DEFAULT_MODEL = PROJECT_DIR / "runs" / "detect" / "train13" / "weights" / "best.pt"
# Jetson Orin Nano 40 Pin 排针 UART1：Pin 8=TX，Pin 10=RX。
# 若实际系统映射不同，仍可通过 --serial-port 临时覆盖。
DEFAULT_SERIAL_PORT = "/dev/ttyTHS1"

# ==================== 1. 形状、颜色和通信常量 ====================
# 形状和颜色的排列顺序必须与 STM32/HMI 的指令表完全一致。
# 指令码计算公式：command = shape_index * 9 + color_index + 1。
SHAPE_ORDER = (
    "regular_tetrahedron",
    "triangular_prism",
    "pentagonal_prism",
    "hexagonal_prism",
    "cylinder",
    "cuboid",
    "cube",
    "sphere",
)
COLOR_ORDER = ("red", "orange", "yellow", "green", "cyan", "blue", "purple", "gray", "black")

# 按模型名称映射；模型的 0..5 类别索引不能直接当作旧串口形状序号。
# cone（圆锥）只显示；triangular pyramid（三棱锥）沿用原 RT 形状条目。
MODEL_SHAPE_MAP = {
    "cone": None,
    "cube": "cube",
    "cuboid": "cuboid",
    "triangular pyramid": "regular_tetrahedron",
    "cylinder": "cylinder",
    "pentagonal prism": "pentagonal_prism",
}
# 白色在旧协议中没有条目；当前只预览，后续根据 STM32 的实际表格再确定。
COLOR_TO_PROTOCOL = {"red": "red", "yellow": "yellow", "green": "green", "blue": "blue"}

PICK_FRAME_TYPE = 0x01
STATUS_FRAME_TYPE = 0x81
STATUS_ACK = 0x01
STATUS_BUSY = 0x02
STATUS_DONE = 0x03
STATUS_FAILED = 0x04
STATUS_OUT_OF_RANGE = 0x05
STATUS_NAMES = {
    STATUS_ACK: "ACK",
    STATUS_BUSY: "BUSY",
    STATUS_DONE: "DONE",
    STATUS_FAILED: "FAILED",
    STATUS_OUT_OF_RANGE: "OUT_OF_RANGE",
}

# 与固件 app_config.h 中的 Y 工作空间上限一致。
MACHINE_Y_MAX_MM = 205.0

# OpenCV HSV 范围：H=0..179，S=0..255，V=0..255。
# 每个颜色可配置多个区间，例如红色跨越 H 轴首尾，需要两个区间。
# 现场光线改变后，主要调整这里的阈值。
COLOR_RANGES = {
    "red": [((0, 65, 35), (12, 255, 255)), ((170, 65, 35), (179, 255, 255))],
    "yellow": [((15, 65, 50), (36, 255, 255))],
    "green": [((37, 50, 30), (89, 255, 255))],
    "blue": [((90, 50, 30), (135, 255, 255))],
    "white": [((0, 0, 145), (179, 65, 255))],
}

DRAW_COLORS = {
    "red": (0, 0, 255),
    "orange": (0, 128, 255),
    "yellow": (0, 255, 255),
    "green": (0, 220, 0),
    "cyan": (255, 255, 0),
    "blue": (255, 80, 0),
    "purple": (220, 0, 220),
    "black": (40, 40, 40),
    "gray": (150, 150, 150),
    "white": (245, 245, 245),
    "unknown": (0, 165, 255),
}


@dataclass
class Detection:
    """一帧画面中的单个识别结果，包含图像坐标、机械坐标和指令码。"""

    color: str
    shape: str
    center: Tuple[int, int]
    box: Tuple[int, int, int, int]
    area: float
    perimeter: float
    vertices: int
    circularity: float
    solidity: float
    machine_center: Optional[Tuple[float, float]] = None
    command: Optional[int] = None
    confidence: float = 0.0
    model_class: str = ""


@dataclass
class LockedTarget:
    """已下发给 STM32 的目标快照；锁定后不再被其他检测目标替换。"""

    sequence: int
    command: int
    color: str
    shape: str
    pixel_center: Tuple[int, int]
    machine_center: Tuple[float, float]


def parse_args() -> argparse.Namespace:
    """读取摄像头、ROI、串口和抓取状态机的命令行参数。"""

    parser = argparse.ArgumentParser(
        description="YOLO shapes + HSV colors; serial communication is disabled by default."
    )
    parser.add_argument("--model", "--weights", type=Path, default=DEFAULT_MODEL, help="YOLO shape-model weights")
    parser.add_argument("--conf", type=float, default=0.5, help="Minimum YOLO confidence")
    parser.add_argument("--iou", type=float, default=0.45, help="YOLO NMS IoU threshold")
    parser.add_argument("--imgsz", type=int, default=640, help="YOLO inference image size")
    parser.add_argument("--device", default=None, help="Inference device, e.g. 0 or cpu; default is automatic")
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--image", type=Path, help="Process one image instead of a camera")
    source.add_argument("--camera", type=int, default=0, help="USB camera index")
    parser.add_argument("--width", type=int, default=1280, help="Camera width")
    parser.add_argument("--height", type=int, default=720, help="Camera height")
    parser.add_argument("--fps", type=int, default=30, help="Requested camera frame rate")
    camera_format = parser.add_mutually_exclusive_group()
    camera_format.add_argument(
        "--mjpeg",
        dest="mjpeg",
        action="store_true",
        default=True,
        help="Request MJPEG camera format (default)",
    )
    camera_format.add_argument(
        "--raw-camera",
        dest="mjpeg",
        action="store_false",
        help="Use the camera default pixel format instead of MJPEG",
    )
    parser.add_argument(
        "--process-scale",
        type=float,
        default=0.5,
        help="HSV color-mask scale (0.25..1.0); YOLO uses the original-resolution color ROI",
    )
    parser.add_argument(
        "--display-scale",
        type=float,
        default=0.5,
        help="Display-only scale in range 0.25..1.0 (default: 0.5); does not affect calibration",
    )
    parser.add_argument(
        "--roi",
        default="saved",
        help="ROI: saved, auto, full, or x,y,w,h (example: 250,60,520,520)",
    )
    parser.add_argument(
        "--roi-file",
        type=Path,
        default=DEFAULT_ROI_FILE,
        help="JSON file used to load/save the selected ROI",
    )
    parser.add_argument(
        "--calibration-file",
        type=Path,
        default=DEFAULT_CALIBRATION_FILE,
        help="Pixel-to-machine calibration JSON",
    )
    parser.add_argument(
        "--serial-port",
        default=None,
        help=f"Optional serial port, e.g. {DEFAULT_SERIAL_PORT} or COM3; disabled by default",
    )
    parser.add_argument(
        "--no-serial",
        action="store_true",
        help="Disable Jetson-to-STM32 serial communication for visual debugging",
    )
    parser.add_argument(
        "--serial-debug",
        action="store_true",
        help="Print raw bytes received from STM32 for protocol debugging",
    )
    parser.add_argument("--baudrate", type=int, default=115200, help="Serial baud rate")
    parser.add_argument("--stable-frames", type=int, default=5, help="Stable frames before sending")
    parser.add_argument("--stable-radius", type=float, default=5.0, help="Stable position radius in mm")
    parser.add_argument("--ack-timeout", type=float, default=1.0, help="ACK timeout in seconds")
    parser.add_argument("--pick-timeout", type=float, default=24.0, help="Pick completion timeout")
    parser.add_argument("--min-area", type=float, default=1800.0, help="Minimum contour area")
    parser.add_argument(
        "--max-area-ratio",
        type=float,
        default=0.20,
        help="Ignore a contour larger than this fraction of the ROI",
    )
    parser.add_argument(
        "--bg-diff",
        type=float,
        default=10.0,
        help="Legacy contour option; YOLO colors are now voted inside each detection box",
    )
    parser.add_argument(
        "--show-masks",
        action="store_true",
        help="Show the color-mask debug window (disabled by default for speed)",
    )
    # 保留旧参数兼容性；新版本默认就不显示掩膜窗口。
    parser.add_argument("--no-masks", action="store_true", help=argparse.SUPPRESS)
    parser.add_argument("--no-display", action="store_true", help="Do not open GUI windows")
    parser.add_argument("--output", type=Path, help="Save the annotated frame to this path")
    args = parser.parse_args()
    if not 0.25 <= args.process_scale <= 1.0:
        parser.error("--process-scale must be between 0.25 and 1.0")
    if not 0.25 <= args.display_scale <= 1.0:
        parser.error("--display-scale must be between 0.25 and 1.0")
    if args.fps <= 0:
        parser.error("--fps must be greater than 0")
    if not 0 < args.conf <= 1 or not 0 < args.iou <= 1:
        parser.error("--conf and --iou must be in range (0, 1]")
    if args.imgsz <= 0:
        parser.error("--imgsz must be positive")
    return args


# ==================== 2. ROI 读取与持久化 ====================
# 兼容旧的 (x,y,w,h) 矩形；按 R 新建的 ROI 为按边缘顺序排列的四个像素顶点。
Roi = Union[Tuple[int, int, int, int], Tuple[Tuple[int, int], ...]]


def roi_bounds(roi: Roi) -> Tuple[int, int, int, int]:
    """四边形只用外接矩形取图，坐标仍在原始画面中，不作透视变换。"""
    values = np.asarray(roi, dtype=np.int32)
    if values.shape == (4,):
        return tuple(int(value) for value in values)
    return cv2.boundingRect(values)


def roi_vertices(roi: Roi) -> np.ndarray:
    values = np.asarray(roi, dtype=np.int32)
    if values.shape == (4,):
        x, y, width, height = roi_bounds(roi)
        return np.array(((x, y), (x + width - 1, y),
                         (x + width - 1, y + height - 1), (x, y + height - 1)), dtype=np.int32)
    return values


def roi_mask(roi: Roi) -> Optional[np.ndarray]:
    if np.asarray(roi).shape == (4,):
        return None
    x, y, width, height = roi_bounds(roi)
    mask = np.zeros((height, width), dtype=np.uint8)
    cv2.fillPoly(mask, [roi_vertices(roi) - (x, y)], 255)
    return mask


def roi_contains(roi: Roi, point: Tuple[int, int]) -> bool:
    return cv2.pointPolygonTest(roi_vertices(roi), (float(point[0]), float(point[1])), False) >= 0


def validate_quad_roi(points: Sequence[Sequence[float]], frame_shape: Sequence[int]) -> Roi:
    values = np.asarray(points, dtype=np.float64)
    frame_height, frame_width = frame_shape[:2]
    if values.shape != (4, 2) or not np.isfinite(values).all():
        raise ValueError("ROI needs four finite points")
    if (values < 0).any() or (values[:, 0] > frame_width - 1).any() or (values[:, 1] > frame_height - 1).any():
        raise ValueError("ROI points must be inside the image")
    values = np.rint(values).astype(np.int32)
    if len(np.unique(values, axis=0)) != 4 or not cv2.isContourConvex(values):
        raise ValueError("Click four corners in order; edges must not cross")
    _, _, width, height = cv2.boundingRect(values)
    if (width < frame_width * 0.10 or height < frame_height * 0.10
            or cv2.contourArea(values) < frame_width * frame_height * 0.01):
        raise ValueError("ROI is too small for object detection")
    return tuple(tuple(int(value) for value in point) for point in values)


def parse_roi(value: str, frame_shape: Sequence[int]) -> Optional[Tuple[int, int, int, int]]:
    if value.lower() in {"saved", "auto", "full"}:
        return None
    try:
        x, y, width, height = (int(part.strip()) for part in value.split(","))
    except ValueError as exc:
        raise ValueError("--roi must be saved, auto, full, or x,y,w,h") from exc

    frame_height, frame_width = frame_shape[:2]
    x = max(0, min(x, frame_width - 1))
    y = max(0, min(y, frame_height - 1))
    width = max(1, min(width, frame_width - x))
    height = max(1, min(height, frame_height - y))
    return x, y, width, height


def save_roi_config(
    roi: Roi,
    frame_shape: Sequence[int],
    path: Path = DEFAULT_ROI_FILE,
) -> None:
    """原子保存四边形顶点；仍可读写旧版矩形配置。"""
    frame_height, frame_width = frame_shape[:2]
    polygon = np.asarray(roi).shape == (4, 2)
    if polygon:
        roi = validate_quad_roi(roi, frame_shape)
    x, y, width, height = roi_bounds(roi)
    center_x_pixel = int(round(x + width / 2.0))
    center_y_pixel = int(round(y + height / 2.0))
    payload = {
        "version": 2,
        "frame_width": frame_width,
        "frame_height": frame_height,
        "coordinate_note": "pixel origin is top-left; +x is right; +y is down",
        "x": x / frame_width,
        "y": y / frame_height,
        "width": width / frame_width,
        "height": height / frame_height,
        "x_pixel": x,
        "y_pixel": y,
        "width_pixel": width,
        "height_pixel": height,
        "center_x": center_x_pixel / frame_width,
        "center_y": center_y_pixel / frame_height,
        "center_x_pixel": center_x_pixel,
        "center_y_pixel": center_y_pixel,
    }
    if polygon:
        payload.update({
            "version": 3,
            "shape": "quadrilateral",
            "points_pixel": [list(point) for point in roi],
            "points_normalized": [[px / frame_width, py / frame_height] for px, py in roi],
        })
    path = path.expanduser().resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary_path = path.with_suffix(path.suffix + ".tmp")
    temporary_path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    temporary_path.replace(path)


def load_roi_config(
    frame_shape: Sequence[int],
    path: Path = DEFAULT_ROI_FILE,
) -> Optional[Roi]:
    """读取四边形或旧矩形；损坏的四边形不会退化成外接矩形。"""

    path = path.expanduser().resolve()
    if not path.is_file():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
        if not isinstance(payload, dict):
            raise ValueError("ROI file must contain an object")
        if payload.get("version") == 3 or "points_normalized" in payload:
            points = np.asarray(payload["points_normalized"], dtype=np.float64)
            if points.shape != (4, 2) or not np.isfinite(points).all() or (points < 0).any() or (points > 1).any():
                raise ValueError("Invalid normalized ROI points")
            frame_height, frame_width = frame_shape[:2]
            points = np.minimum(np.rint(points * (frame_width, frame_height)), (frame_width - 1, frame_height - 1))
            return validate_quad_roi(points, frame_shape)
        values = [float(payload[key]) for key in ("x", "y", "width", "height")]
    except (OSError, KeyError, TypeError, ValueError, json.JSONDecodeError) as exc:
        print(f"Ignoring invalid ROI file {path}: {exc}")
        return None

    if any(not 0.0 <= value <= 1.0 for value in values) or values[2] <= 0 or values[3] <= 0:
        print(f"Ignoring invalid normalized ROI values in {path}")
        return None
    if values[2] < 0.10 or values[3] < 0.10:
        print(f"Ignoring ROI that is too small for object detection: {path}")
        return None

    frame_height, frame_width = frame_shape[:2]
    x = int(round(values[0] * frame_width))
    y = int(round(values[1] * frame_height))
    width = int(round(values[2] * frame_width))
    height = int(round(values[3] * frame_height))
    return parse_roi(f"{x},{y},{width},{height}", frame_shape)


def select_quad_roi(
    frame: np.ndarray,
    display_scale: float = 1.0,
    service: Optional[Callable[[], None]] = None,
) -> Optional[Roi]:
    """按边缘顺序点击四角，Enter/空格确认，右键/退格撤回，Esc 取消。"""
    window = "Select four ROI corners"
    height, width = frame.shape[:2]
    display_width = max(1, int(round(width * display_scale)))
    display_height = max(1, int(round(height * display_scale)))
    background = cv2.resize(frame, (display_width, display_height), interpolation=cv2.INTER_AREA)
    points: List[Tuple[int, int]] = []
    cursor: Optional[Tuple[int, int]] = None
    message = ""

    def on_mouse(event, x, y, flags, userdata):
        nonlocal cursor, message
        cursor = (max(0, min(int(x), display_width - 1)), max(0, min(int(y), display_height - 1)))
        if event == cv2.EVENT_LBUTTONDOWN and len(points) < 4:
            points.append((min(width - 1, int(round(cursor[0] * width / display_width))),
                           min(height - 1, int(round(cursor[1] * height / display_height)))))
            message = ""
        elif event == cv2.EVENT_RBUTTONDOWN and points:
            points.pop()
            message = ""

    cv2.namedWindow(window, cv2.WINDOW_AUTOSIZE)
    cv2.setMouseCallback(window, on_mouse)
    try:
        while True:
            if service is not None:
                service()
            canvas = background.copy()
            scaled = np.rint(np.asarray(points).reshape(-1, 2) * (display_width / width, display_height / height)).astype(np.int32)
            if len(points) == 4:
                overlay = canvas.copy()
                cv2.fillPoly(overlay, [scaled], (0, 160, 0))
                canvas = cv2.addWeighted(canvas, 0.75, overlay, 0.25, 0)
            if len(points) >= 2:
                cv2.polylines(canvas, [scaled], len(points) == 4, (0, 255, 255), 2)
            if points and len(points) < 4 and cursor is not None:
                cv2.line(canvas, tuple(scaled[-1]), cursor, (0, 255, 255), 1)
            for index, point in enumerate(scaled):
                cv2.circle(canvas, tuple(point), 4, (0, 0, 255), -1)
                cv2.putText(canvas, str(index + 1), (int(point[0]) + 7, max(15, int(point[1]) - 7)),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2, cv2.LINE_AA)
            lines = [f"Click corners in order: {len(points)}/4 | Enter/Space: save",
                     "Right click/Backspace: undo | R: reset | Esc/Q: cancel"]
            if message:
                lines.append(message)
            for index, text in enumerate(lines):
                origin = (8, 22 + index * 23)
                cv2.putText(canvas, text, origin, cv2.FONT_HERSHEY_SIMPLEX, 0.48, (0, 0, 0), 3, cv2.LINE_AA)
                cv2.putText(canvas, text, origin, cv2.FONT_HERSHEY_SIMPLEX, 0.48, (0, 255, 255), 1, cv2.LINE_AA)
            cv2.imshow(window, canvas)
            key = cv2.waitKey(20) & 0xFF
            if key in (27, ord("q")):
                return None
            try:
                if cv2.getWindowProperty(window, cv2.WND_PROP_VISIBLE) < 1:
                    return None
            except cv2.error:
                return None
            if key in (8, 127) and points:
                points.pop()
                message = ""
            elif key == ord("r"):
                points.clear()
                message = ""
            elif key in (10, 13, 32):
                try:
                    return validate_quad_roi(points, frame.shape)
                except ValueError as exc:
                    message = str(exc)
    finally:
        try:
            cv2.destroyWindow(window)
        except cv2.error:
            pass


# ==================== 3. 坐标转换与形状颜色编码 ====================
def encode_hmi_command(shape: str, color: str) -> int:
    """按形状序号和颜色序号生成 1..72 的单字节业务指令码。"""
    try:
        shape_index = SHAPE_ORDER.index(shape)
    except ValueError as exc:
        raise ValueError(f"Unknown 3D shape: {shape}") from exc
    try:
        color_index = COLOR_ORDER.index(color)
    except ValueError as exc:
        raise ValueError(f"Unknown protocol color: {color}") from exc
    return shape_index * len(COLOR_ORDER) + color_index + 1


def load_calibration(path: Path = DEFAULT_CALIBRATION_FILE) -> Optional[np.ndarray]:
    """读取像素点和机械坐标标定点，计算像素平面到机械平面的单应矩阵。"""

    path = path.expanduser().resolve()
    if not path.is_file():
        print(f"Calibration file not found: {path}")
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
        image_points = np.asarray(payload["image_points_px"], dtype=np.float32)
        machine_points = np.asarray(payload["machine_points_mm"], dtype=np.float32)
        if image_points.shape[0] < 4 or image_points.shape != machine_points.shape:
            raise ValueError("calibration needs at least four matching 2D point pairs")
        # 4 个点可直接求透视变换；多于 4 个点时用 RANSAC 抑制异常标定点。
        if image_points.shape[0] == 4:
            homography = cv2.getPerspectiveTransform(image_points, machine_points)
        else:
            homography, _ = cv2.findHomography(image_points, machine_points, cv2.RANSAC)
        if homography is None or not np.isfinite(homography).all():
            raise ValueError("cannot calculate a valid homography")
    except (OSError, KeyError, TypeError, ValueError, json.JSONDecodeError) as exc:
        print(f"Ignoring invalid calibration file {path}: {exc}")
        return None

    projected = cv2.perspectiveTransform(image_points.reshape(-1, 1, 2), homography).reshape(-1, 2)
    errors = np.linalg.norm(projected - machine_points, axis=1)
    payload["homography"] = homography.tolist()
    payload["reprojection_error_mm"] = [float(value) for value in errors]
    payload["mean_reprojection_error_mm"] = float(np.mean(errors))
    path.write_text(json.dumps(payload, indent=2), encoding="utf-8")
    print(
        f"Loaded calibration: {path} | mean point error "
        f"{payload['mean_reprojection_error_mm']:.3f} mm"
    )
    return homography


def pixel_to_machine(
    pixel: Tuple[int, int], homography: Optional[np.ndarray]
) -> Optional[Tuple[float, float]]:
    """将物体中心 (u, v) 像素坐标转换为机械臂使用的 (x, y) 毫米坐标。"""

    if homography is None:
        return None
    point = np.asarray([[pixel]], dtype=np.float32)
    converted = cv2.perspectiveTransform(point, homography)[0, 0]
    if not np.isfinite(converted).all():
        return None
    # 先限幅再筛选/显示，避免超过 Y 行程的目标被丢弃。
    return float(converted[0]), min(float(converted[1]), MACHINE_Y_MAX_MM)


def resolve_protocol_shape(projection: str, color: str) -> Optional[str]:
    """根据 YOLO 的形状名称查旧协议；圆锥等未定义类别不会被猜测映射。"""
    normalized = " ".join(projection.strip().lower().replace("_", " ").split())
    return MODEL_SHAPE_MAP.get(normalized)


class YoloShapeDetector:
    """启动时只加载一次权重，检测时始终使用原始分辨率的彩色 ROI。"""

    def __init__(self, weights: Path, confidence: float, iou: float, image_size: int, device=None):
        weights = weights.expanduser().resolve()
        if not weights.is_file():
            raise FileNotFoundError(f"YOLO weights not found: {weights}; specify --model with best.pt")
        # 优先复用这个工程里的 Ultralytics，避免与训练时的代码版本不一致。
        source_dir = PROJECT_DIR / "YOLOv8_source"
        if (source_dir / "ultralytics" / "__init__.py").is_file():
            sys.path.insert(0, str(source_dir))
        from ultralytics import YOLO

        self.model = YOLO(str(weights))
        if self.model.task != "detect":
            raise ValueError(f"Expected detection weights, got task={self.model.task}")
        self.names = self.model.names
        self.options = dict(conf=confidence, iou=iou, imgsz=image_size, verbose=False, save=False)
        if device is not None:
            self.options["device"] = device
        print(f"Loaded YOLO shape model: {weights}")
        print(f"YOLO classes: {self.names}")

    def predict(self, color_roi: np.ndarray) -> np.ndarray:
        # OpenCV 的 BGR 原图直接传入即可，YOLO 内部完成通道转换与缩放。
        result = self.model.predict(source=color_roi, **self.options)[0]
        if result.boxes is None:
            return np.empty((0, 6), dtype=np.float32)
        return result.boxes.data.cpu().numpy()


# ==================== 4. Jetson 与 STM32 串口通信 ====================
# Jetson -> STM32：AA 55 TYPE SEQ CMD X_L X_H Y_L Y_H CHECK，共 10 字节。
# STM32 -> Jetson：55 AA TYPE SEQ STATUS CHECK，共 6 字节。
def build_pick_packet(sequence: int, command: int, x_mm: float, y_mm: float) -> bytes:
    """把任务序号、形状颜色指令码和毫米坐标封装成 10 字节抓取帧。"""
    if not 0 <= sequence <= 255:
        raise ValueError("sequence must be in range 0..255")
    if not 1 <= command <= 72:
        raise ValueError("command must be in range 1..72")
    if not math.isfinite(x_mm) or not math.isfinite(y_mm):
        raise ValueError("coordinates must be finite")
    x_value = int(round(x_mm))
    y_value = int(round(min(y_mm, MACHINE_Y_MAX_MM)))
    if not 0 <= x_value <= 65535 or not 0 <= y_value <= 65535:
        raise ValueError(f"coordinates out of uint16 range: ({x_value}, {y_value})")
    # < 表示小端序，HH 表示 X/Y 各使用 2 字节无符号整数。
    body = struct.pack(
        "<BBBBBHH",
        0xAA,
        0x55,
        PICK_FRAME_TYPE,
        sequence,
        command,
        x_value,
        y_value,
    )
    return body + bytes((sum(body) & 0xFF,))


def build_status_packet(sequence: int, status: int) -> bytes:
    """构造 STM32 状态帧，主要供协议测试和 STM32 端实现时参考。"""
    if not 0 <= sequence <= 255:
        raise ValueError("sequence must be in range 0..255")
    if status not in STATUS_NAMES:
        raise ValueError(f"unknown status: {status}")
    body = bytes((0x55, 0xAA, STATUS_FRAME_TYPE, sequence, status))
    return body + bytes((sum(body) & 0xFF,))


class SerialLink:
    """负责打开串口、发送抓取帧，并从字节流中解析 STM32 状态帧。"""

    def __init__(self, port: str, baudrate: int, debug: bool = False) -> None:
        try:
            import serial
        except ImportError as exc:
            raise RuntimeError(
                "pyserial is required for --serial-port; install it with: pip3 install pyserial"
            ) from exc
        self.connection = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=8,
            parity="N",
            stopbits=1,
            timeout=0,
            write_timeout=0.2,
        )
        self.debug = debug
        self.receive_buffer = bytearray()
        print(f"Serial opened: {port}, {baudrate}, 8N1")

    def send_pick(self, target: LockedTarget) -> None:
        """发送一个已锁定目标；重发时沿用相同 sequence，防止重复抓取。"""

        packet = build_pick_packet(
            target.sequence,
            target.command,
            target.machine_center[0],
            target.machine_center[1],
        )
        self.connection.write(packet)
        print(
            f"TX pick: seq={target.sequence} cmd={target.command} "
            f"xy=({target.machine_center[0]:.1f}, {target.machine_center[1]:.1f}) mm "
            f"frame={packet.hex(' ')}"
        )

    def poll_status(self) -> List[Tuple[int, int]]:
        """非阻塞读取串口，完成帧头同步、校验和检查和状态提取。"""

        waiting = self.connection.in_waiting
        if waiting:
            received = self.connection.read(waiting)
            if self.debug:
                print(f"RX raw: {received.hex(' ')}")
            self.receive_buffer.extend(received)

        messages = []
        while len(self.receive_buffer) >= 6:
            header_index = self.receive_buffer.find(b"\x55\xAA")
            if header_index < 0:
                self.receive_buffer.clear()
                break
            if header_index:
                del self.receive_buffer[:header_index]
            if len(self.receive_buffer) < 6:
                break

            packet = bytes(self.receive_buffer[:6])
            if packet[2] != STATUS_FRAME_TYPE or (sum(packet[:5]) & 0xFF) != packet[5]:
                del self.receive_buffer[0]
                continue
            del self.receive_buffer[:6]
            sequence, status = packet[3], packet[4]
            if status in STATUS_NAMES:
                messages.append((sequence, status))
                print(f"RX status: seq={sequence} status={STATUS_NAMES[status]}")
        return messages

    def close(self) -> None:
        self.connection.close()


# ==================== 5. 单目标抓取状态机 ====================
class PickController:
    """一次只锁定一个目标；机械动作期间暂停识别，DONE 后再恢复。"""

    def __init__(
        self,
        serial_link: SerialLink,
        stable_frames: int,
        stable_radius_mm: float,
        ack_timeout: float,
        pick_timeout: float,
    ) -> None:
        self.serial_link = serial_link
        self.stable_frames = max(1, stable_frames)
        self.stable_radius_mm = stable_radius_mm
        self.ack_timeout = ack_timeout
        self.pick_timeout = pick_timeout
        self.state = "DETECT"
        self.state_started = time.monotonic()
        self.status_text = "waiting for stable target"
        self.candidate: Optional[Detection] = None
        self.candidate_frames = 0
        self.target: Optional[LockedTarget] = None
        self.sequence = 0
        self.retry_count = 0
        self.last_tx_time = 0.0
        self.last_gripper_position = (0.0, 0.0)
        self.failed_targets: List[Tuple[int, float, float, float]] = []

    @staticmethod
    def _distance(a: Tuple[float, float], b: Tuple[float, float]) -> float:
        return math.hypot(a[0] - b[0], a[1] - b[1])

    def _is_failed_target(self, detection: Detection, now: float) -> bool:
        self.failed_targets = [item for item in self.failed_targets if item[3] > now]
        if detection.machine_center is None or detection.command is None:
            return True
        # 冷却按物理位置判断，不比较 command。否则同一物体在相邻帧被识别成
        # 不同形状时会绕过冷却，导致机械臂立即重复抓取同一位置。
        return any(
            self._distance(detection.machine_center, (x_mm, y_mm)) <= 30.0
            for _command, x_mm, y_mm, _ in self.failed_targets
        )

    def _select_target(self, detections: List[Detection], now: float) -> Optional[Detection]:
        """从可发送目标中选择周围更空旷、机械臂移动距离较短的目标。"""

        sendable = [
            detection
            for detection in detections
            if detection.command is not None
            and detection.machine_center is not None
            and not self._is_failed_target(detection, now)
        ]
        if not sendable:
            return None

        all_positions = [
            detection.machine_center
            for detection in detections
            if detection.machine_center is not None
        ]

        def priority(detection: Detection) -> Tuple[float, float, float]:
            position = detection.machine_center
            assert position is not None
            neighbor_distances = [
                self._distance(position, other)
                for other in all_positions
                if self._distance(position, other) > 1e-6
            ]
            clearance = min(neighbor_distances) if neighbor_distances else 1000.0
            travel = self._distance(position, self.last_gripper_position)
            # clearance 越大越不容易碰到其他物体；travel 只作为较小的移动代价。
            score = clearance - 0.15 * travel
            return score, -position[1], -position[0]

        return max(sendable, key=priority)

    def _same_candidate(self, first: Detection, second: Detection) -> bool:
        return (
            first.command == second.command
            and first.machine_center is not None
            and second.machine_center is not None
            and self._distance(first.machine_center, second.machine_center) <= self.stable_radius_mm
        )

    def _send_candidate(self, detection: Detection, now: float) -> None:
        """锁定稳定目标并发送一次，随后进入 WAIT_ACK，停止选择其他目标。"""

        assert detection.command is not None and detection.machine_center is not None
        self.sequence = (self.sequence + 1) & 0xFF
        self.target = LockedTarget(
            sequence=self.sequence,
            command=detection.command,
            color=detection.color,
            shape=detection.shape,
            pixel_center=detection.center,
            machine_center=detection.machine_center,
        )
        self.retry_count = 0
        self.serial_link.send_pick(self.target)
        self.last_tx_time = now
        self.state = "WAIT_ACK"
        self.state_started = now
        self.status_text = f"waiting ACK seq={self.sequence}"
        self.candidate = None
        self.candidate_frames = 0

    def _complete_target(self, now: float) -> None:
        """STM32 返回 DONE 后完成任务，并从下一帧恢复识别。"""

        assert self.target is not None
        print(f"Pick completed: seq={self.target.sequence}")
        self.last_gripper_position = self.target.machine_center
        self.target = None
        self.state = "DETECT"
        self.state_started = now
        self.status_text = "DONE; detecting next"
        self.candidate = None
        self.candidate_frames = 0

    def _fail(self, reason: str, now: float) -> None:
        print(f"Pick failed: {reason}")
        if self.target is not None:
            x_mm, y_mm = self.target.machine_center
            self.failed_targets.append((self.target.command, x_mm, y_mm, now + 10.0))
        self.state = "ERROR"
        self.state_started = now
        self.status_text = reason
        self.candidate = None
        self.candidate_frames = 0

    def update(self, detections: List[Detection], now: Optional[float] = None) -> None:
        """每帧调用一次，根据识别结果和 STM32 回包推进抓取状态。"""

        now = time.monotonic() if now is None else now
        statuses = self.serial_link.poll_status()

        # 只处理当前锁定任务的 SEQ，避免旧任务的延迟回包干扰新任务。
        for sequence, status in statuses:
            if self.target is None or sequence != self.target.sequence:
                continue
            if status in (STATUS_ACK, STATUS_BUSY) and self.state == "WAIT_ACK":
                self.state = "PICKING"
                self.state_started = now
                self.status_text = STATUS_NAMES[status].lower()
            elif status == STATUS_DONE and self.state in ("WAIT_ACK", "PICKING"):
                self._complete_target(now)
            elif status in (STATUS_FAILED, STATUS_OUT_OF_RANGE) and self.state in (
                "WAIT_ACK",
                "PICKING",
            ):
                self._fail(STATUS_NAMES[status], now)

        if self.state == "DETECT":
            # 目标必须在若干连续帧内保持稳定，防止噪声轮廓触发误抓。
            selected = self._select_target(detections, now)
            if selected is None:
                self.candidate = None
                self.candidate_frames = 0
                self.status_text = "no sendable target"
                return
            if self.candidate is not None and self._same_candidate(self.candidate, selected):
                self.candidate_frames += 1
            else:
                self.candidate_frames = 1
            self.candidate = selected
            self.status_text = (
                f"stabilizing {self.candidate_frames}/{self.stable_frames} "
                f"cmd={selected.command}"
            )
            if self.candidate_frames >= self.stable_frames:
                self._send_candidate(selected, now)

        elif self.state == "WAIT_ACK":
            # ACK 超时只重发一次，并沿用相同 SEQ；STM32 应把重复 SEQ 当作同一任务。
            if now - self.last_tx_time >= self.ack_timeout:
                if self.retry_count < 1 and self.target is not None:
                    self.retry_count += 1
                    self.serial_link.send_pick(self.target)
                    self.last_tx_time = now
                    self.status_text = f"ACK timeout; retry {self.retry_count}"
                else:
                    self._fail("ACK timeout", now)

        elif self.state == "PICKING":
            # 已收到 ACK/BUSY，此阶段只等待 DONE，不会发送下一个目标。
            if now - self.state_started >= self.pick_timeout:
                self._fail("pick timeout", now)

        elif self.state == "ERROR" and now - self.state_started >= 2.0:
            self.target = None
            self.state = "DETECT"
            self.state_started = now
            self.status_text = "error cooldown complete"

    def draw_status(self, image: np.ndarray) -> None:
        text = f"Pick: AUTO | State: {self.state} | {self.status_text}"
        cv2.putText(image, text, (12, 58), cv2.FONT_HERSHEY_SIMPLEX, 0.62, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.putText(image, text, (12, 58), cv2.FONT_HERSHEY_SIMPLEX, 0.62, (0, 255, 255), 1, cv2.LINE_AA)
        if self.target is not None:
            center = self.target.pixel_center
            cv2.drawMarker(
                image,
                center,
                (0, 0, 255),
                markerType=cv2.MARKER_CROSS,
                markerSize=32,
                thickness=3,
            )
            cv2.putText(
                image,
                f"LOCKED seq={self.target.sequence}",
                (center[0] + 18, center[1] - 18),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.65,
                (0, 0, 255),
                2,
                cv2.LINE_AA,
            )


# ==================== 6. OpenCV 图像预处理与轮廓识别 ====================
def detect_tray_roi(frame: np.ndarray) -> Optional[Tuple[int, int, int, int]]:
    """优先通过长直线寻找托盘边界，失败时再根据托盘颜色估计区域。"""
    frame_height, frame_width = frame.shape[:2]
    frame_area = frame_height * frame_width
    gray = cv2.cvtColor(cv2.GaussianBlur(frame, (5, 5), 0), cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, 40, 130)
    min_line_length = int(min(frame_height, frame_width) * 0.25)
    lines = cv2.HoughLinesP(
        edges,
        1,
        np.pi / 180,
        threshold=70,
        minLineLength=min_line_length,
        maxLineGap=45,
    )

    horizontal = []
    vertical = []
    if lines is not None:
        for x1, y1, x2, y2 in lines[:, 0]:
            delta_x = x2 - x1
            delta_y = y2 - y1
            length = math.hypot(delta_x, delta_y)
            angle = abs(math.degrees(math.atan2(delta_y, delta_x))) % 180
            if angle <= 10 or angle >= 170:
                horizontal.append((0.5 * (y1 + y2), length, min(x1, x2), max(x1, x2)))
            elif 80 <= angle <= 100:
                vertical.append((0.5 * (x1 + x2), length, min(y1, y2), max(y1, y2)))

    best_box = None
    best_score = -float("inf")
    minimum_tray_side = 0.38 * max(frame_width, frame_height)
    for top in horizontal:
        for bottom in horizontal:
            y1, y2 = sorted((top[0], bottom[0]))
            height = y2 - y1
            if height < 0.25 * frame_height:
                continue
            for left in vertical:
                for right in vertical:
                    x1, x2 = sorted((left[0], right[0]))
                    width = x2 - x1
                    if width < minimum_tray_side or height < minimum_tray_side:
                        continue
                    area = width * height
                    if not 0.15 * frame_area <= area <= 0.70 * frame_area:
                        continue
                    aspect = width / height
                    if not 0.65 <= aspect <= 1.45:
                        continue

                    horizontal_coverage = (
                        min(top[1], width) / width + min(bottom[1], width) / width
                    )
                    vertical_coverage = (
                        min(left[1], height) / height + min(right[1], height) / height
                    )
                    square_score = -3.0 * abs(math.log(aspect))
                    center_x = 0.5 * (x1 + x2)
                    center_y = 0.5 * (y1 + y2)
                    center_penalty = math.hypot(
                        (center_x - frame_width / 2) / frame_width,
                        (center_y - frame_height / 2) / frame_height,
                    )
                    score = horizontal_coverage + vertical_coverage + square_score - center_penalty
                    if score > best_score:
                        best_score = score
                        best_box = (int(x1), int(y1), int(width), int(height))

    if best_box is not None:
        x, y, width, height = best_box
        inset = max(2, int(min(width, height) * 0.005))
        return x + inset, y + inset, width - 2 * inset, height - 2 * inset

    # Fallback for images where the tray edges are not sufficiently visible.
    blurred = cv2.GaussianBlur(frame, (7, 7), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    tray_mask = cv2.inRange(hsv, (5, 35, 35), (42, 255, 255))
    tray_mask = cv2.morphologyEx(
        tray_mask, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (21, 21))
    )

    contours, _ = cv2.findContours(tray_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        return None

    candidates = [contour for contour in contours if cv2.contourArea(contour) > 0.08 * frame_area]
    if not candidates:
        return None

    x, y, width, height = cv2.boundingRect(max(candidates, key=cv2.contourArea))
    if width * height > 0.92 * frame_area:
        return None

    inset = max(4, int(min(width, height) * 0.025))
    x += inset
    y += inset
    width -= 2 * inset
    height -= 2 * inset
    if width <= 0 or height <= 0:
        return None
    return x, y, width, height


def estimate_tray_lab(roi: np.ndarray) -> Optional[np.ndarray]:
    """从 ROI 内边缘取样，估计白色/低饱和度托盘的 Lab 背景颜色。"""
    height, width = roi.shape[:2]
    border = max(5, int(min(height, width) * 0.08))
    sample_mask = np.zeros((height, width), dtype=np.uint8)
    sample_mask[:border, :] = 255
    sample_mask[-border:, :] = 255
    sample_mask[:, :border] = 255
    sample_mask[:, -border:] = 255

    hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)
    neutral_background = cv2.inRange(hsv, (0, 0, 100), (179, 100, 255))
    neutral_samples = cv2.bitwise_and(sample_mask, neutral_background)
    if cv2.countNonZero(neutral_samples) >= 100:
        sample_mask = neutral_samples
    lab = cv2.cvtColor(roi, cv2.COLOR_BGR2LAB).astype(np.float32)
    pixels = lab[sample_mask > 0]
    if len(pixels) < 100:
        return None
    return np.median(pixels, axis=0)


def make_foreground_mask(roi: np.ndarray, min_lab_distance: float) -> np.ndarray:
    """计算每个像素与托盘背景色的 Lab 距离，分离物体和背景。"""

    if min_lab_distance <= 0:
        return np.full(roi.shape[:2], 255, dtype=np.uint8)

    tray_lab = estimate_tray_lab(roi)
    if tray_lab is None:
        return np.full(roi.shape[:2], 255, dtype=np.uint8)

    lab = cv2.cvtColor(roi, cv2.COLOR_BGR2LAB).astype(np.float32)
    distance = np.linalg.norm(lab - tray_lab, axis=2)
    foreground = np.where(distance >= min_lab_distance, 255, 0).astype(np.uint8)
    foreground = cv2.morphologyEx(
        foreground, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    )
    return foreground


def make_color_masks(
    roi: np.ndarray,
    background_mask: np.ndarray,
    processing_scale: float = 1.0,
) -> Dict[str, np.ndarray]:
    """按 HSV 阈值生成五种颜色的掩膜，供 YOLO 框内投票；不判断几何形状。"""

    # ROI 缩小时同步缩小形态学核，保持它在原图上的实际作用范围基本不变。
    def scaled_odd_size(base_size: int) -> int:
        size = max(3, int(round(base_size * processing_scale)))
        return size if size % 2 == 1 else size + 1

    blur_size = scaled_odd_size(5)
    blurred = cv2.GaussianBlur(roi, (blur_size, blur_size), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
    open_size = scaled_odd_size(5)
    close_size = scaled_odd_size(9)
    open_kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (open_size, open_size))
    close_kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (close_size, close_size))
    masks = {}

    for color_name, ranges in COLOR_RANGES.items():
        mask = np.zeros(hsv.shape[:2], dtype=np.uint8)
        for lower, upper in ranges:
            mask = cv2.bitwise_or(
                mask,
                cv2.inRange(hsv, np.array(lower, dtype=np.uint8), np.array(upper, dtype=np.uint8)),
            )
        mask = cv2.bitwise_and(mask, background_mask)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, open_kernel)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, close_kernel)
        masks[color_name] = mask

    return masks


def classify_box_color(
    masks: Dict[str, np.ndarray],
    box: Tuple[float, float, float, float],
    roi_size: Tuple[int, int],
) -> str:
    """只在检测框内部统计 HSV；优先使用彩色像素，避免黑托盘和白色反光干扰。"""
    roi_width, roi_height = roi_size
    mask_height, mask_width = next(iter(masks.values())).shape
    x1, y1, x2, y2 = box
    # 向内缩 10%，减少检测框四角的托盘、相邻物体和边缘混合像素。
    inset_x, inset_y = (x2 - x1) * 0.10, (y2 - y1) * 0.10
    left = max(0, int(math.floor((x1 + inset_x) * mask_width / roi_width)))
    top = max(0, int(math.floor((y1 + inset_y) * mask_height / roi_height)))
    right = min(mask_width, int(math.ceil((x2 - inset_x) * mask_width / roi_width)))
    bottom = min(mask_height, int(math.ceil((y2 - inset_y) * mask_height / roi_height)))
    if right <= left or bottom <= top:
        return "unknown"
    counts = {name: cv2.countNonZero(mask[top:bottom, left:right]) for name, mask in masks.items()}
    inner_area = (right - left) * (bottom - top)
    chromatic = sorted(((counts[name], name) for name in ("red", "yellow", "green", "blue")), reverse=True)
    count, color = chromatic[0]
    # 白色物料的阴影/反射可能略偏蓝或偏绿。白色占绝对多数时，
    # 不让少量带色像素把整个白色物料改判为蓝色或绿色。
    if counts["white"] >= max(8, inner_area * 0.55) and counts["white"] >= 2.0 * count:
        return "white"
    # 彩色物体的高光可能落入白色阈值，因此有足够彩色证据时不被高光改成白色。
    if count >= max(8, inner_area * 0.05):
        runner_up = chromatic[1][0]
        if runner_up >= inner_area * 0.15 and count < runner_up * 1.2:
            return "unknown"
        return color
    if counts["white"] >= max(8, inner_area * 0.20):
        return "white"
    return "unknown"


# 以下两个轮廓函数仅保留为旧版参考，当前 process_frame 只调用 YOLO。
def classify_projection(contour: np.ndarray) -> Tuple[str, int, float, float]:
    """根据凸包顶点数、圆度和四边形边长比判断二维投影形状。"""

    area = cv2.contourArea(contour)
    perimeter = cv2.arcLength(contour, True)
    if perimeter <= 0:
        return "unknown", 0, 0.0, 0.0

    hull = cv2.convexHull(contour)
    hull_area = cv2.contourArea(hull)
    solidity = area / hull_area if hull_area > 0 else 0.0
    circularity = 4.0 * math.pi * area / (perimeter * perimeter)

    # Use the convex hull to reduce small notches caused by glare and shadows.
    epsilon = 0.025 * cv2.arcLength(hull, True)
    approx = cv2.approxPolyDP(hull, epsilon, True)
    vertices = len(approx)

    if circularity >= 0.78 and vertices >= 7:
        shape = "circle"
    elif vertices == 3:
        shape = "triangle"
    elif vertices == 4:
        rect = cv2.minAreaRect(hull)
        side_a, side_b = rect[1]
        ratio = min(side_a, side_b) / max(side_a, side_b) if max(side_a, side_b) > 0 else 0
        shape = "square" if ratio >= 0.82 else "rectangle"
    elif vertices == 5:
        shape = "pentagon"
    elif vertices == 6:
        shape = "hexagon"
    elif vertices >= 7:
        shape = "round" if circularity >= 0.65 else "polygon"
    else:
        shape = "unknown"

    return shape, vertices, circularity, solidity


def find_detections(
    masks: Dict[str, np.ndarray],
    roi_offset: Tuple[int, int],
    min_area: float,
    max_area: float,
    coordinate_scale: Tuple[float, float] = (1.0, 1.0),
) -> List[Detection]:
    """提取轮廓，并将缩小 ROI 中的结果换算回原始画面坐标。"""

    offset_x, offset_y = roi_offset
    scale_x, scale_y = coordinate_scale
    area_scale = scale_x * scale_y
    perimeter_scale = 0.5 * (scale_x + scale_y)
    detections = []

    for color_name, mask in masks.items():
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            area = cv2.contourArea(contour)
            if area < min_area or area > max_area:
                continue

            local_x, local_y, local_width, local_height = cv2.boundingRect(contour)
            mask_height, mask_width = mask.shape
            border_margin = 2
            if (
                local_x <= border_margin
                or local_y <= border_margin
                or local_x + local_width >= mask_width - border_margin
                or local_y + local_height >= mask_height - border_margin
            ):
                continue

            shape, vertices, circularity, solidity = classify_projection(contour)
            if solidity < 0.68:
                continue

            # 使用轮廓矩计算重心，再换算回完整分辨率下的像素坐标。
            moments = cv2.moments(contour)
            if abs(moments["m00"]) < 1e-6:
                continue
            center_x = int(round((moments["m10"] / moments["m00"]) * scale_x)) + offset_x
            center_y = int(round((moments["m01"] / moments["m00"]) * scale_y)) + offset_y
            detections.append(
                Detection(
                    color=color_name,
                    shape=shape,
                    center=(center_x, center_y),
                    box=(
                        int(round(local_x * scale_x)) + offset_x,
                        int(round(local_y * scale_y)) + offset_y,
                        int(round(local_width * scale_x)),
                        int(round(local_height * scale_y)),
                    ),
                    area=area * area_scale,
                    perimeter=cv2.arcLength(contour, True) * perimeter_scale,
                    vertices=vertices,
                    circularity=circularity,
                    solidity=solidity,
                )
            )

    # A shadow/highlight can split one object into two color masks. If one box is
    # mostly contained in another, keep the more complete (larger-area) contour.
    deduplicated = []
    for candidate in sorted(detections, key=lambda item: item.area, reverse=True):
        candidate_x, candidate_y, candidate_width, candidate_height = candidate.box
        candidate_box_area = candidate_width * candidate_height
        is_duplicate = False
        for kept in deduplicated:
            kept_x, kept_y, kept_width, kept_height = kept.box
            intersection_width = max(
                0,
                min(candidate_x + candidate_width, kept_x + kept_width)
                - max(candidate_x, kept_x),
            )
            intersection_height = max(
                0,
                min(candidate_y + candidate_height, kept_y + kept_height)
                - max(candidate_y, kept_y),
            )
            intersection = intersection_width * intersection_height
            smaller_box_area = min(candidate_box_area, kept_width * kept_height)
            if smaller_box_area > 0 and intersection / smaller_box_area >= 0.60:
                is_duplicate = True
                break
        if not is_duplicate:
            deduplicated.append(candidate)

    return sorted(deduplicated, key=lambda item: (item.color, item.center[1], item.center[0]))


def draw_detections(
    frame: np.ndarray,
    masks: Dict[str, np.ndarray],
    detections: List[Detection],
    roi: Roi,
) -> np.ndarray:
    """在画面上绘制 ROI、检测框、中心点、机械坐标和指令码。"""

    result = frame.copy()
    cv2.polylines(result, [roi_vertices(roi)], True, (0, 255, 255), 2)

    for detection in detections:
        x, y, width, height = detection.box
        color = DRAW_COLORS[detection.color]
        cv2.rectangle(result, (x, y), (x + width, y + height), color, 2)
        cv2.circle(result, detection.center, 4, (0, 0, 255), -1)
        position_text = f"px=({detection.center[0]},{detection.center[1]})"
        if detection.machine_center is not None:
            position_text += f" mm=({detection.machine_center[0]:.1f},{detection.machine_center[1]:.1f})"
        command_text = "" if detection.command is None else f" cmd={detection.command}"
        label = f"{detection.color} {detection.model_class or detection.shape} {detection.confidence:.2f} {position_text}{command_text}"
        text_y = max(22, y - 7)
        cv2.putText(
            result,
            label,
            (x, text_y),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.55,
            (0, 0, 0),
            3,
            cv2.LINE_AA,
        )
        cv2.putText(
            result,
            label,
            (x, text_y),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.55,
            color,
            1,
            cv2.LINE_AA,
        )

    cv2.putText(
        result,
        f"YOLO objects: {len(detections)} | R: 4-point ROI | S: save | Q: quit",
        (12, 28),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.65,
        (0, 0, 0),
        3,
        cv2.LINE_AA,
    )
    cv2.putText(
        result,
        f"YOLO objects: {len(detections)} | R: 4-point ROI | S: save | Q: quit",
        (12, 28),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.65,
        (0, 255, 0),
        1,
        cv2.LINE_AA,
    )
    return result


def make_mask_grid(masks: Dict[str, np.ndarray], cell_width: int = 260) -> np.ndarray:
    cells = []
    for name, mask in masks.items():
        height, width = mask.shape
        cell_height = max(1, int(height * cell_width / width))
        cell = cv2.resize(mask, (cell_width, cell_height), interpolation=cv2.INTER_NEAREST)
        cell = cv2.cvtColor(cell, cv2.COLOR_GRAY2BGR)
        cv2.putText(cell, name, (8, 23), cv2.FONT_HERSHEY_SIMPLEX, 0.65, DRAW_COLORS[name], 2)
        cells.append(cell)

    columns = 3
    rows = []
    for start in range(0, len(cells), columns):
        row = cells[start : start + columns]
        while len(row) < columns:
            row.append(np.zeros_like(cells[0]))
        rows.append(np.hstack(row))
    return np.vstack(rows)


def process_frame(
    frame: np.ndarray,
    roi: Roi,
    min_area: float,
    max_area_ratio: float,
    background_distance: float,
    processing_scale: float = 1.0,
    homography: Optional[np.ndarray] = None,
    draw_result: bool = True,
    detector: Optional[YoloShapeDetector] = None,
) -> Tuple[np.ndarray, Dict[str, np.ndarray], List[Detection]]:
    """YOLO 判断形状，框内 HSV 判断颜色；输出原图像素中心及机械坐标。"""

    if detector is None:
        raise ValueError("YOLO detector is required; the contour classifier is not used")
    x, y, width, height = roi_bounds(roi)
    full_roi = frame[y : y + height, x : x + width]
    polygon_mask = roi_mask(roi)
    # 形状使用原始彩色 ROI。预测框由 YOLO 自动还原到 full_roi 的像素尺寸，
    # 不能再次乘以 --process-scale，否则抓取中心坐标会错位。
    model_image = full_roi if polygon_mask is None else cv2.bitwise_and(full_roi, full_roi, mask=polygon_mask)
    predictions = detector.predict(model_image)

    # 缩小只用于 HSV 五色统计，减轻 CPU 负担，不影响模型输入和标定坐标。
    processed_width = max(1, int(round(width * processing_scale)))
    processed_height = max(1, int(round(height * processing_scale)))
    if processed_width == width and processed_height == height:
        roi_image = full_roi
    else:
        roi_image = cv2.resize(
            full_roi,
            (processed_width, processed_height),
            interpolation=cv2.INTER_AREA,
        )

    # 框已经由 YOLO 给出，无需先按托盘颜色去背景，避免把白色物料一并滤掉。
    color_region = (np.full(roi_image.shape[:2], 255, dtype=np.uint8) if polygon_mask is None else
                    cv2.resize(polygon_mask, (processed_width, processed_height), interpolation=cv2.INTER_NEAREST))
    masks = make_color_masks(roi_image, color_region, processing_scale)
    if polygon_mask is not None:
        masks = {name: cv2.bitwise_and(mask, color_region) for name, mask in masks.items()}
    detections = []
    for prediction in predictions:
        x1, y1, x2, y2 = [float(value) for value in prediction[:4]]
        x1, x2 = max(0.0, x1), min(float(width), x2)
        y1, y2 = max(0.0, y1), min(float(height), y2)
        box_width, box_height = x2 - x1, y2 - y1
        area = box_width * box_height
        if box_width <= 0 or box_height <= 0 or area < min_area or area > width * height * max_area_ratio:
            continue
        # 当前是检测模型而非分割模型，中心采用检测框中心，不再用颜色轮廓重心。
        center = (
            x + min(width - 1, int(round((x1 + x2) / 2.0))),
            y + min(height - 1, int(round((y1 + y2) / 2.0))),
        )
        if polygon_mask is not None and not roi_contains(roi, center):
            continue
        class_id = int(prediction[-1])
        model_class = str(detector.names[class_id])
        color = classify_box_color(masks, (x1, y1, x2, y2), (width, height))
        protocol_shape = resolve_protocol_shape(model_class, color)
        left, top = int(math.floor(x1)), int(math.floor(y1))
        detection = Detection(
            color=color,
            shape=protocol_shape or model_class,
            center=center,
            box=(x + left, y + top, int(math.ceil(x2)) - left, int(math.ceil(y2)) - top),
            area=area,
            perimeter=2.0 * (box_width + box_height),
            vertices=0,
            circularity=0.0,
            solidity=1.0,
            machine_center=pixel_to_machine(center, homography),
            confidence=float(prediction[-2]),
            model_class=model_class,
        )
        protocol_color = COLOR_TO_PROTOCOL.get(color)
        valid_xy = detection.machine_center is not None and all(
            0 <= round(value) <= 65535 for value in detection.machine_center
        )
        if protocol_shape is not None and protocol_color is not None and valid_xy:
            detection.command = encode_hmi_command(protocol_shape, protocol_color)
        detections.append(detection)
    # 比赛无窗口模式不创建全分辨率副本，也不绘制方框和文字。
    result = draw_detections(frame, masks, detections, roi) if draw_result else frame
    return result, masks, detections


# ==================== 7. 摄像头、画面显示与程序主循环 ====================
def open_camera(
    index: int, width: int, height: int, fps: int, use_mjpeg: bool
) -> cv2.VideoCapture:
    """使用平台原生后端打开 USB 摄像头，并设置低延迟单帧缓冲。"""

    backend = cv2.CAP_V4L2 if sys.platform.startswith("linux") else cv2.CAP_DSHOW
    capture = cv2.VideoCapture(index, backend)
    if not capture.isOpened():
        capture.release()
        capture = cv2.VideoCapture(1)
    if not capture.isOpened():
        raise RuntimeError(f"Cannot open camera {index}; try --camera 0 or --camera 2")

    # MJPEG 可降低 USB 带宽，但某些 Jetson/OpenCV 组合会用 CPU 解码而变慢，
    # 因此只在命令行显式传入 --mjpeg 时启用。
    if use_mjpeg:
        capture.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))
    capture.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    capture.set(cv2.CAP_PROP_FPS, fps)
    capture.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    return capture


def save_result(path: Path, image: np.ndarray) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if not cv2.imwrite(str(path), image):
        raise RuntimeError(f"Failed to save image: {path}")
    print(f"Saved: {path}")


def main() -> None:
    """初始化模型、摄像头和标定，默认仅显示识别结果，不连接 STM32。"""

    args = parse_args()
    detector = YoloShapeDetector(args.model, args.conf, args.iou, args.imgsz, args.device)
    capture = None
    serial_link = None
    pick_controller = None

    if args.image:
        image_path = args.image.expanduser().resolve()
        frame = cv2.imread(str(image_path))
        if frame is None:
            raise FileNotFoundError(f"Cannot read image: {image_path}")
        is_single_image = True
    else:
        capture = open_camera(args.camera, args.width, args.height, args.fps, args.mjpeg)
        ok, frame = capture.read()
        if not ok:
            capture.release()
            raise RuntimeError("Cannot read the first camera frame")
        is_single_image = False
        camera_reported_fps = capture.get(cv2.CAP_PROP_FPS)

    roi_mode = args.roi.lower()
    configured_roi = parse_roi(args.roi, frame.shape)
    saved_roi = load_roi_config(frame.shape, args.roi_file) if roi_mode == "saved" else None
    manual_roi = saved_roi or configured_roi
    auto_roi_enabled = roi_mode == "auto" or (roi_mode == "saved" and saved_roi is None)
    detected_auto_roi = detect_tray_roi(frame) if auto_roi_enabled else None
    # 标定矩阵只需在启动时计算一次，之后每帧复用。
    homography = load_calibration(args.calibration_file)
    # 当前默认只预览。只有明确指定 --serial-port 才保留旧通信入口。
    if args.serial_port and not args.no_serial and not is_single_image:
        serial_link = SerialLink(args.serial_port, args.baudrate, debug=args.serial_debug)
        pick_controller = PickController(
            serial_link=serial_link,
            stable_frames=args.stable_frames,
            stable_radius_mm=args.stable_radius,
            ack_timeout=args.ack_timeout,
            pick_timeout=args.pick_timeout,
        )
    else:
        print("Preview mode: serial is OFF; no pick commands will be sent.")
    last_result = None
    last_frame_time: Optional[float] = None
    smoothed_fps: Optional[float] = None
    smoothed_capture_ms: Optional[float] = None
    smoothed_process_ms: Optional[float] = None
    smoothed_ui_ms: Optional[float] = None

    if saved_roi is not None:
        print(f"Loaded ROI from: {args.roi_file.expanduser().resolve()} -> {saved_roi}")
    elif roi_mode == "saved":
        print(f"No saved ROI found at {args.roi_file.expanduser().resolve()}; using auto ROI")

    if not args.no_display:
        cv2.namedWindow(WINDOW_RESULT, cv2.WINDOW_NORMAL)
        if args.show_masks and not args.no_masks:
            cv2.namedWindow(WINDOW_MASKS, cv2.WINDOW_NORMAL)

    try:
        while True:
            frame_started = time.perf_counter()
            capture_ms = 0.0
            if capture is not None:
                capture_started = time.perf_counter()
                ok, frame = capture.read()
                capture_ms = (time.perf_counter() - capture_started) * 1000.0
                if not ok:
                    print("Camera frame read failed")
                    break

            if manual_roi is not None:
                roi = manual_roi
            elif auto_roi_enabled:
                roi = detected_auto_roi or (0, 0, frame.shape[1], frame.shape[0])
            else:
                roi = (0, 0, frame.shape[1], frame.shape[0])

            draw_result = not args.no_display or args.output is not None
            vision_active = pick_controller is None or pick_controller.state == "DETECT"
            if vision_active:
                # 只有 DETECT 状态执行颜色和形状识别。发送任务后仍持续读取摄像头，
                # 但跳过识别，避免机械爪运动产生的轮廓干扰候选目标。
                process_started = time.perf_counter()
                result, masks, detections = process_frame(
                    frame,
                    roi,
                    min_area=args.min_area,
                    max_area_ratio=args.max_area_ratio,
                    background_distance=args.bg_diff,
                    processing_scale=args.process_scale,
                    homography=homography,
                    draw_result=draw_result,
                    detector=detector,
                )
                process_ms = (time.perf_counter() - process_started) * 1000.0
            else:
                detections = []
                masks = {}
                result = frame.copy() if draw_result else frame
                if draw_result:
                    cv2.polylines(result, [roi_vertices(roi)], True, (0, 255, 255), 2)
                process_ms = 0.0
            if pick_controller is not None:
                # 串口模式下由状态机决定何时发送；不会每帧重复发送。
                pick_controller.update(detections)
                if draw_result:
                    pick_controller.draw_status(result)

            if not is_single_image and draw_result:
                # 统计完整循环帧率（摄像头读取、识别、串口轮询和显示），并进行平滑。
                current_frame_time = time.perf_counter()
                frame_seconds = (
                    max(current_frame_time - last_frame_time, 1e-6)
                    if last_frame_time is not None
                    else max(current_frame_time - frame_started, 1e-6)
                )
                instant_fps = 1.0 / frame_seconds
                last_frame_time = current_frame_time
                smoothed_fps = (
                    instant_fps
                    if smoothed_fps is None
                    else 0.85 * smoothed_fps + 0.15 * instant_fps
                )
                smoothed_capture_ms = (
                    capture_ms
                    if smoothed_capture_ms is None
                    else 0.85 * smoothed_capture_ms + 0.15 * capture_ms
                )
                smoothed_process_ms = (
                    process_ms
                    if smoothed_process_ms is None
                    else 0.85 * smoothed_process_ms + 0.15 * process_ms
                )
                fps_text = (
                    f"FPS {smoothed_fps:.1f} | CAP {smoothed_capture_ms:.0f} ms | "
                    f"CV {smoothed_process_ms:.0f} ms | "
                    f"UI {(smoothed_ui_ms or 0.0):.0f} ms | CAM {camera_reported_fps:.0f}"
                )
                (fps_width, _), _ = cv2.getTextSize(
                    fps_text, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2
                )
                fps_origin = (12, max(20, result.shape[0] - 12))
                cv2.putText(
                    result,
                    fps_text,
                    fps_origin,
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (0, 0, 0),
                    4,
                    cv2.LINE_AA,
                )
                cv2.putText(
                    result,
                    fps_text,
                    fps_origin,
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7,
                    (0, 255, 255),
                    2,
                    cv2.LINE_AA,
                )
            last_result = result

            if args.output:
                save_result(args.output.expanduser().resolve(), result)
                args.output = None

            if args.no_display:
                # 摄像头模式下关闭窗口但继续识别、串口通信和抓取状态机；
                # 单张图片模式处理完一帧即可退出。
                if is_single_image:
                    break
                continue

            ui_started = time.perf_counter()
            if args.display_scale < 1.0:
                display_result = cv2.resize(
                    result,
                    None,
                    fx=args.display_scale,
                    fy=args.display_scale,
                    interpolation=cv2.INTER_AREA,
                )
            else:
                display_result = result
            cv2.imshow(WINDOW_RESULT, display_result)
            if args.show_masks and not args.no_masks and masks:
                cv2.imshow(WINDOW_MASKS, make_mask_grid(masks))

            key = cv2.waitKey(0 if is_single_image else 1) & 0xFF
            ui_ms = (time.perf_counter() - ui_started) * 1000.0
            smoothed_ui_ms = (
                ui_ms
                if smoothed_ui_ms is None
                else 0.85 * smoothed_ui_ms + 0.15 * ui_ms
            )
            if key in (ord("q"), 27):
                break
            if key == ord("r"):
                if pick_controller is not None and pick_controller.state in ("WAIT_ACK", "PICKING"):
                    print("ROI selection waits until the current pick and HOME are complete")
                    continue
                selected = select_quad_roi(
                    frame, args.display_scale,
                    (lambda: pick_controller.update([])) if pick_controller is not None else None,
                )
                if selected is not None:
                    try:
                        save_roi_config(selected, frame.shape, args.roi_file)
                    except OSError as exc:
                        print(f"Could not save ROI; keeping previous selection: {exc}")
                    else:
                        manual_roi = selected
                        auto_roi_enabled = False
                        print(f"ROI updated: {manual_roi}")
                        print(f"ROI saved to: {args.roi_file.expanduser().resolve()}")
            elif key == ord("c"):
                manual_roi = None
                auto_roi_enabled = False
            elif key == ord("s") and last_result is not None:
                save_result(Path(__file__).with_name("cv2_detection_result.jpg"), last_result)

            if is_single_image and key not in (ord("r"), ord("c"), ord("s")):
                break
    except KeyboardInterrupt:
        print("Stopped by user")
    finally:
        if serial_link is not None:
            serial_link.close()
        if capture is not None:
            capture.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
