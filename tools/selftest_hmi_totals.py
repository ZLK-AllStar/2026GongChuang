"""Check the real Frm02 CUSTOM methods with a minimal list-widget stand-in."""
from pathlib import Path
import argparse
import subprocess

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_HMI = Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/"
                   "ApusIDE2_v20250930/workspace/TEST")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hmi", type=Path, default=DEFAULT_HMI)
    args = parser.parse_args()
    out = ROOT / "MDK-ARM/MechanicalVerify/hmi_totals_test"
    out.mkdir(parents=True, exist_ok=True)
    source = (args.hmi / "src/pages/Frm02.cpp").read_text(encoding="utf-8")
    custom = source.split("/*CUSTOM_CODE_BEGIN*/", 1)[1].split("/*CUSTOM_CODE_END*/", 1)[0]
    (out / "Frm02_custom.inc").write_text(custom, encoding="utf-8")
    exe = out / "test_hmi_totals.exe"
    subprocess.run(["g++", "-static", "-std=c++11", "-Wall", "-Wextra",
                    "-I" + str(out), "-I" + str(args.hmi / "src"),
                    str(ROOT / "tests/mechanical/test_hmi_totals.cpp"),
                    "-o", str(exe)], check=True)
    subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
