"""Test real Wcmn method bodies with host SDK stand-ins; no panel deployment.

Only SDK includes/namespaces in Wcmn.h are replaced. Wcmn.cpp is copied
unchanged. This checks application behavior, not the proprietary SDK runtime.
"""
from pathlib import Path
import argparse
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_HMI = Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930/workspace/TEST")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hmi", type=Path, default=DEFAULT_HMI)
    args = parser.parse_args()
    out = ROOT / "MDK-ARM/MechanicalVerify/hmi_worker_test"
    out.mkdir(parents=True, exist_ok=True)
    source = args.hmi / "src"
    header = (source / "pages/Wcmn.h").read_text(encoding="utf-8-sig")
    header = re.sub(r'^#include[^\n]*$', '', header, flags=re.MULTILINE)
    header = re.sub(r'^using namespace stt[^\n]*$', '', header, flags=re.MULTILINE)
    (out / "Wcmn.h").write_text('#include "hmi_worker_mock.h"\n' + header, encoding="utf-8")
    (out / "Frm02.h").write_text('#include "hmi_worker_mock.h"\n', encoding="utf-8")
    shutil.copy2(source / "pages/Wcmn.cpp", out / "Wcmn.cpp")
    exe = out / "test_hmi_worker.exe"
    subprocess.run(["g++", "-static", "-std=c++11", "-Wall", "-Wextra",
                    "-I" + str(out), "-I" + str(source),
                    "-I" + str(ROOT / "tests/mechanical"),
                    str(out / "Wcmn.cpp"), str(ROOT / "tests/mechanical/test_hmi_worker.cpp"),
                    "-o", str(exe)], check=True)
    subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()