#!/usr/bin/env bash
# 开机自启入口：由 huatai-vision.service 调用，也可手动执行用于前台调试。
#
# 职责：
#   1. 切到 classic-new 目录（_cv2.py 用 __file__ 定位 config/）；
#   2. 激活 yolov8_env 虚拟环境（内含 ultralytics/torch/OpenCV/pyserial）；
#   3. 以后台方式启动 _cv2.py：不创建 GUI 窗口，识别结果只写日志。
#
# 覆盖参数可放 /etc/default/huatai-vision：
#   HUATAI_VISION_ARGS="--camera 1 --serial-port /dev/ttyTHS1"

set -euo pipefail

# 用法：
#   scripts/run_vision.sh                    开机/服务启动路径（默认，参数为 --no-display）
#   scripts/run_vision.sh --debug --image x   手动调试：先停服务，再以前台运行并透传参数
#
# 手动调试必须停掉服务：摄像头同一时刻只能有一个进程取流，服务在跑时
# 手动启动会拿到 "Device is busy"，看起来像摄像头坏了。
RUN_DEBUG=0
if [ "${1:-}" = "--debug" ]; then
  RUN_DEBUG=1
  shift
fi
PASSTHROUGH_ARGS="$*"

# yolov8_env/bin/activate 会 source ~/yolov8_env/bin/setup_jetson_env.sh，其中
# 直接引用了 $LD_LIBRARY_PATH / $PYTHONPATH / $PATH。systemd 的环境比登录
# shell 干净得多，这些变量可能根本不存在；先给出空默认值，避免在
# `set -u` 下激活失败（交互式 shell 里因为已 export 过所以不会暴露）。
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}"
export PYTHONPATH="${PYTHONPATH:-}"
export PATH="${PATH:-/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin}"

PROJECT_DIR="${HUATAI_PROJECT_DIR:-/home/jetson/classic-new}"
VENV_DIR="${HUATAI_VENV_DIR:-/home/jetson/yolov8_env}"
PYTHON="${HUATAI_PYTHON:-python3}"
SCRIPT="${HUATAI_SCRIPT:-_cv2.py}"

# 无 HDMI / 不接显示器时的默认参数：关闭 OpenCV 窗口，保留串口与抓取状态机。
# 摄像头索引用自动探测（--camera 只作为首选），换 USB 口后无需改配置。
DEFAULT_ARGS="--no-display"
EXTRA_ARGS=""

# 开机时 USB 摄像头可能还没枚举完，先等一下再启动，避免首帧读不到。
CAMERA_STARTUP_DELAY_S=5

if [ -r /etc/default/huatai-vision ]; then
  # shellcheck disable=SC1091
  . /etc/default/huatai-vision
  EXTRA_ARGS="${HUATAI_VISION_ARGS:-}"
fi

if [ ! -d "${PROJECT_DIR}" ]; then
  echo "huatai-vision: project directory not found: ${PROJECT_DIR}" >&2
  exit 1
fi

cd "${PROJECT_DIR}"

if [ ! -f "${VENV_DIR}/bin/activate" ]; then
  echo "huatai-vision: virtualenv activate script not found: ${VENV_DIR}/bin/activate" >&2
  exit 1
fi

# shellcheck disable=SC1091
. "${VENV_DIR}/bin/activate"

if [ "${RUN_DEBUG}" -eq 1 ]; then
  # 调试模式：停掉常驻服务，解除摄像头占用，然后前台跑，日志直接打在终端。
  if systemctl is-active --quiet huatai-vision 2>/dev/null; then
    if [ -t 0 ]; then
      echo "huatai-vision: stopping the service so the camera is free"
      sudo systemctl stop huatai-vision
    else
      # 没有终端时 sudo 无法读密码（例如从管道/自动化里调用），给出明确指令。
      echo "huatai-vision: the service is still running and owns the camera." >&2
      echo "huatai-vision: stop it first: sudo systemctl stop huatai-vision" >&2
      exit 1
    fi
  fi
  # 调试模式优先显示画面：只有显式传了 --no-display、或没有可用的 X 显示
  # （例如纯 SSH 没有 X11 转发）时，才退回无窗口模式。
  case " ${PASSTHROUGH_ARGS} " in
    *" --no-display "*|*" -h "*|*" --help "*)
      DEBUG_ARGS="${PASSTHROUGH_ARGS}"
      ;;
    *)
      if [ -n "${DISPLAY:-}" ]; then
        echo "huatai-vision: debug display: DISPLAY=${DISPLAY}"
        DEBUG_ARGS="${PASSTHROUGH_ARGS}"
      else
        echo "huatai-vision: DISPLAY is not set; running without windows." >&2
        echo "huatai-vision: for the GUI use X11 forwarding (ssh -X / MobaXterm X server)" >&2
        echo "huatai-vision: or set DISPLAY manually, e.g. DISPLAY=:0 $0 --debug" >&2
        DEBUG_ARGS="--no-display ${PASSTHROUGH_ARGS}"
      fi
      ;;
  esac
  echo "huatai-vision: debug start: ${PYTHON} -u ${SCRIPT} ${DEBUG_ARGS}"
  echo "huatai-vision: venv=$(command -v python) dir=$(pwd)"
  exec "${PYTHON}" -u "${SCRIPT}" ${DEBUG_ARGS}
fi

echo "huatai-vision: running ${PYTHON} ${SCRIPT} ${DEFAULT_ARGS} ${EXTRA_ARGS}"
echo "huatai-vision: venv=$(command -v python) dir=$(pwd)"

# -u 让日志逐行写入 journal，避免缓冲区吞掉按键和任务进度输出。
# 开机场景下 USB 摄像头可能还没枚举完，先等一段时间再打开（默认 0 秒，
# systemd 单元会通过 HUATAI_CAMERA_DELAY_S 设为 5 秒）。
if [ "${HUATAI_CAMERA_DELAY_S:-0}" -gt 0 ]; then
  echo "huatai-vision: waiting ${HUATAI_CAMERA_DELAY_S}s for the USB camera"
  sleep "${HUATAI_CAMERA_DELAY_S}"
fi

exec "${PYTHON}" -u "${SCRIPT}" ${DEFAULT_ARGS} ${EXTRA_ARGS}
