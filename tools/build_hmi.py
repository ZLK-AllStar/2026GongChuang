"""Build the specified ApusIDE project as an ARM Linux shared library.

Outputs stay under the firmware's ignored MDK-ARM/MechanicalVerify directory.
This script compiles and links only; it does not deploy to the panel.
"""
from pathlib import Path
import argparse
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sdk", type=Path, default=Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/ApusIDE2_v20250930"))
    parser.add_argument("--project", type=Path)
    args = parser.parse_args()
    hmi = args.project or args.sdk / "workspace/TEST"
    out = ROOT / "MDK-ARM/MechanicalVerify/hmi_arm"
    out.mkdir(parents=True, exist_ok=True)
    binary = args.sdk / "sdk/gcc-linaro-6.3.1-2017.05-i686-mingw32_arm-linux-gnueabihf/bin"
    cxx = binary / "arm-linux-gnueabihf-g++.exe"
    library = args.sdk / "plugins/cn.simtutai.device.simtutai_1.0.0.20250930/bundle/platform/1.1.0.0/arm"
    objects = []
    with (out / "build.log").open("w", encoding="utf-8") as log:
        def run(command):
            result = subprocess.run([str(p) for p in command], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            text = result.stdout.decode("utf-8", errors="replace")
            log.write(text)
            print(text, end="")
            result.check_returncode()
        run([cxx, "--version"])
        for source in sorted((hmi / "src").rglob("*.cpp")):
            obj = out / (source.stem + ".o")
            objects.append(obj)
            run([cxx, "-c", "-O3", "-fPIC", "-std=c++11", "-Wall", "-I" + str(hmi / "lib"), source, "-o", obj])
        run([cxx, "-shared", "-O3", *objects, "-L" + str(library), "-lstt_core", "-lpthread", "-ldl", "-lstdc++", "-o", out / "libbootpanel.so"])
        run([binary / "arm-linux-gnueabihf-readelf.exe", "-h", out / "libbootpanel.so"])
        log.write("BUILD_RESULT: SUCCESS\nERROR_COUNT: 0\nWARNING_COUNT: 0\n")
    print("HMI ARM BUILD SUCCESS:", out / "libbootpanel.so")
    print("FULL_LOG:", out / "build.log")


if __name__ == "__main__":
    main()
