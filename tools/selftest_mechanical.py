"""Run actual firmware C and the specified HMI parser against UART/HAL mocks.

Requires GCC/G++ on PATH. No device access, deployment or package installation.
"""
from pathlib import Path
import argparse
import platform
import subprocess

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_HMI = Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930/workspace/TEST")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--group", choices=["all", "host"], default="all")
    parser.add_argument("--hmi", type=Path, default=DEFAULT_HMI)
    args = parser.parse_args()
    out = ROOT / "MDK-ARM/MechanicalVerify"
    out.mkdir(parents=True, exist_ok=True)
    common = ["-std=c99", "-Wall", "-Wextra", "-Itests/mechanical/stubs", "-ICore/Inc"]
    groups = {
        "tray": [], "boot": [], "lift": [],
        "host": [f"Core/Src/{n}.c" for n in ["host_link", "app_controller", "app_runtime", "classification", "pick_place", "hmi_link"]],
        "shell": ["letter-shell-3.1.2/src/shell.c", "letter-shell-3.1.2/src/shell_ext.c"],
        "soft_uart": [],
        "shell_wire": ["letter-shell-3.1.2/src/shell.c", "letter-shell-3.1.2/src/shell_ext.c"],
    }
    for name, sources in groups.items():
        if args.group == "host" and name != "host":
            continue
        exe = out / f"test_{name}.exe"
        options = ["-Iletter-shell-3.1.2/src"] if name in ("shell", "shell_wire") else []
        if name in ("soft_uart", "shell_wire"):
            options += ["-Itests/mechanical/soft_uart_stubs"]
        # The library's generic function-pointer ABI assumes a 32-bit target;
        # native tests exercise CMD_MAIN without calling that conversion path.
        if name in ("shell", "shell_wire"):
            options += ["-Wno-pointer-to-int-cast", "-Wno-int-to-pointer-cast", "-Wno-unused-parameter", "-Wno-sign-compare"]
            if platform.system() == "Windows":
                options += ["-mno-ms-bitfields"]
        subprocess.run(["gcc", *options, *common, f"tests/mechanical/test_{name}.c", *sources, "-lm", "-o", str(exe)], cwd=ROOT, check=True)
        for scenario in (range(11) if name == "host" else [None]):
            subprocess.run([str(exe), *([] if scenario is None else [str(scenario)])], cwd=ROOT, check=True)
    if args.group == "host":
        return
    exe = out / "test_corexy.exe"
    subprocess.run(["gcc", "-std=c99", "-Wall", "-Wextra", "-Itests/corexy/stubs", "-ICore/Inc", "tests/corexy/test_corexy.c", "-lm", "-o", str(exe)], cwd=ROOT, check=True)
    subprocess.run([str(exe)], check=True)
    hmi_src = args.hmi / "src"
    if (hmi_src / "hmi_protocol.h").read_bytes() != (ROOT / "Core/Inc/hmi_protocol.h").read_bytes():
        raise RuntimeError("Firmware/HMI shared CRC headers differ")
    if (hmi_src / "hmi_codes.h").read_bytes() != (ROOT / "Core/Inc/hmi_codes.h").read_bytes():
        raise RuntimeError("Firmware/HMI shared code mappings differ")
    exe = out / "test_hmi.exe"
    subprocess.run(["g++", "-static", "-std=c++11", "-Wall", "-Wextra", "-Itests/mechanical/stubs", "-ICore/Inc", "-I" + str(hmi_src), "tests/mechanical/test_hmi.cpp", "Core/Src/hmi_link.c", "-o", str(exe)], cwd=ROOT, check=True)
    subprocess.run([str(exe)], check=True)
    print("ALL MECHANICAL REGRESSIONS PASSED")


if __name__ == "__main__":
    main()
