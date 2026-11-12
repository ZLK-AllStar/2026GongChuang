"""Tests the current firmware parser and app_controller.c with UART/HAL mocks."""
import subprocess
import sys
from pathlib import Path
if __name__ == "__main__":
    subprocess.run([sys.executable, str(Path(__file__).with_name("selftest_mechanical.py")), "--group", "host"], check=True)
