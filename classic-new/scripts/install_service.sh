#!/usr/bin/env bash
# 安装/更新 huatai-vision 开机自启服务。需要在 Jetson 上以 sudo 运行：
#   sudo classic-new/scripts/install_service.sh
#
# 做的事：
#   1. 给 run_vision.sh 加可执行权限；
#   2. 安装 systemd 单元到 /etc/systemd/system/；
#   3. daemon-reload 并 enable，使开机自动启动；
#   4. 若服务已在运行则重启一次，让新代码立即生效。

set -euo pipefail

SERVICE_NAME="huatai-vision.service"
PROJECT_DIR="${HUATAI_PROJECT_DIR:-/home/jetson/classic-new}"
SOURCE_UNIT="${PROJECT_DIR}/scripts/${SERVICE_NAME}"
TARGET_UNIT="/etc/systemd/system/${SERVICE_NAME}"

if [ "$(id -u)" -ne 0 ]; then
  echo "run with sudo: sudo $0" >&2
  exit 1
fi

if [ ! -f "${SOURCE_UNIT}" ]; then
  echo "service unit not found: ${SOURCE_UNIT}" >&2
  exit 1
fi

chmod +x "${PROJECT_DIR}/scripts/run_vision.sh"
install -m 0644 "${SOURCE_UNIT}" "${TARGET_UNIT}"

systemctl daemon-reload
systemctl enable "${SERVICE_NAME}"

if systemctl is-active --quiet "${SERVICE_NAME}"; then
  systemctl restart "${SERVICE_NAME}"
else
  systemctl start "${SERVICE_NAME}" || true
fi

sleep 2
systemctl --no-pager --full status "${SERVICE_NAME}" || true
