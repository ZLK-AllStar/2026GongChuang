#!/usr/bin/env bash
# 停止并移除 huatai-vision 开机自启服务（保留项目文件）。需要 sudo：
#   sudo classic-new/scripts/uninstall_service.sh

set -euo pipefail

SERVICE_NAME="huatai-vision.service"
TARGET_UNIT="/etc/systemd/system/${SERVICE_NAME}"

if [ "$(id -u)" -ne 0 ]; then
  echo "run with sudo: sudo $0" >&2
  exit 1
fi

systemctl disable --now "${SERVICE_NAME}" || true
rm -f "${TARGET_UNIT}"
systemctl daemon-reload
systemctl reset-failed "${SERVICE_NAME}" || true
echo "removed ${TARGET_UNIT}"
