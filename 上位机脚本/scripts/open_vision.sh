#!/usr/bin/env bash
# 在 Jetson 桌面终端运行；请等当前抓取和回 HOME 完成后再切换。
set -euo pipefail

usage()
{
  cat <<'EOF'
用法：
  open_vision.sh            打开识别窗口，保留串口与 PE2 抓取控制
  open_vision.sh --preview  只看识别画面，不发送机械指令
  open_vision.sh --help     显示帮助

在 Jetson 桌面或远程桌面终端运行，不要加 sudo。
脚本自动停止后台服务；按 q / Esc 或 Ctrl+C 退出后恢复原先运行的服务。
首次 sudo 验证可能需要输入 Jetson 登录密码。
EOF
}

preview=0
case "${1:-}" in
  "") ;;
  --preview) preview=1; shift ;;
  -h|--help) usage; exit 0 ;;
  *) usage >&2; exit 2 ;;
esac
if (( $# > 0 )); then
  usage >&2
  exit 2
fi

if (( EUID == 0 )); then
  echo "请用 jetson 用户直接运行此脚本，不要在前面加 sudo。" >&2
  exit 1
fi
if [[ -z "${DISPLAY:-}" ]]; then
  echo "没有图形显示环境。请在 Jetson 桌面或远程桌面中打开终端再运行。" >&2
  echo "SSH 查看窗口需先启用 X11 转发；当前后台服务保持原状。" >&2
  exit 1
fi

for command_name in systemctl sudo flock xdpyinfo timeout; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    echo "缺少命令：${command_name}；尚未停止后台服务。" >&2
    exit 1
  fi
done
if ! timeout 5 xdpyinfo -display "$DISPLAY" >/dev/null 2>&1; then
  echo "无法连接显示器 ${DISPLAY}，请从已登录的图形桌面终端运行。" >&2
  exit 1
fi

project_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
venv_dir="/home/jetson/yolov8_env"
service="huatai-vision.service"
if [[ ! -f "${project_dir}/cv2_copy.py" || ! -x "${venv_dir}/bin/python3" ]]; then
  echo "缺少 cv2_copy.py 或 yolov8_env，请把脚本放在 classic/scripts/ 中。" >&2
  exit 1
fi

lock_dir="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}"
if [[ ! -d "$lock_dir" ]]; then
  lock_dir="${HOME}/.cache"
  mkdir -p -- "$lock_dir"
fi
exec 9>"${lock_dir}/huatai-vision-gui.lock"
if ! flock -n 9; then
  echo "识别窗口脚本已在运行，请先退出已有窗口。" >&2
  exit 1
fi

# Jetson 的 activate 附带 CUDA 配置，引用这些环境变量时要求它们存在。
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:-}"
export PYTHONPATH="${PYTHONPATH:-}"
cd -- "$project_dir"
# shellcheck disable=SC1091
source "${venv_dir}/bin/activate"

service_state="$(systemctl show "$service" --property=ActiveState --value)"
case "$service_state" in
  active|activating|reloading) restore_service=1 ;;
  inactive|failed) restore_service=0 ;;
  *) echo "后台服务状态为 ${service_state}，请稍后重试。" >&2; exit 1 ;;
esac

vision_pid=""
sudo_refresh_pid=""
cleanup()
{
  local exit_code=$?
  trap - EXIT
  trap '' INT TERM HUP
  set +e
  # 先确保前台程序释放摄像头与串口，再恢复后台服务。
  if [[ -n "$vision_pid" ]]; then
    kill -TERM "$vision_pid" 2>/dev/null
    wait "$vision_pid" 2>/dev/null
  fi
  if (( restore_service )); then
    echo "正在恢复后台识别服务……"
    if sudo -n systemctl start "$service" && systemctl is-active --quiet "$service"; then
      echo "后台服务已启动；识别前请先松开 PE2，再按一次。"
    else
      echo "恢复失败，请执行：sudo systemctl start ${service}" >&2
      exit_code=1
    fi
  fi
  if [[ -n "$sudo_refresh_pid" ]]; then
    kill "$sudo_refresh_pid" 2>/dev/null
    wait "$sudo_refresh_pid" 2>/dev/null
  fi
  exit "$exit_code"
}

if (( restore_service )); then
  # 停服务前完成验证，长时间看画面时维持凭据，供退出时自动恢复。
  sudo -v
fi
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
trap 'exit 129' HUP
if (( restore_service )); then
  (
    trap - EXIT INT TERM HUP
    refresh_sleep_pid=""
    trap '[[ -z "$refresh_sleep_pid" ]] || kill "$refresh_sleep_pid" 2>/dev/null; exit 0' INT TERM HUP
    while true; do
      sleep 45 &
      refresh_sleep_pid=$!
      wait "$refresh_sleep_pid" || break
      refresh_sleep_pid=""
      sudo -n -v || break
    done
  ) 9>&- </dev/null >/dev/null 2>&1 &
  sudo_refresh_pid=$!
  echo "正在停止后台识别服务……"
  sudo systemctl stop "$service"
fi

vision_args=(--camera 0)
if (( preview )); then
  vision_args+=(--no-serial)
  echo "只看识别画面，不发送机械指令。"
else
  vision_args+=(--serial-port /dev/ttyTHS1 --baudrate 115200)
  echo "窗口打开后，请先松开 PE2，再按一次启动识别和抓取。"
fi
echo "在画面窗口按 q / Esc，或在终端按 Ctrl+C 退出。"
"${venv_dir}/bin/python3" -u "${project_dir}/cv2_copy.py" "${vision_args[@]}" <&0 &
vision_pid=$!
vision_exit_code=0
wait "$vision_pid" || vision_exit_code=$?
vision_pid=""
exit "$vision_exit_code"
