"""Compile the real CoreXY C module with UART/HAL mocks and run it on Unicorn.

Prerequisites: ARM Compiler 5 (Keil), Python, unicorn==2.1.4.
No packages are installed by this script. All outputs are under ignored MDK-ARM/.
"""
from pathlib import Path
import argparse
import struct
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--keil-root', type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    out = root / 'MDK-ARM/CoreXYVerify'
    out.mkdir(parents=True, exist_ok=True)
    compiler = args.keil_root / 'ARM/ARMCC/bin'
    source = root / 'tests/corexy'
    common = ['--cpu', 'Cortex-M4', '--fpu=SoftVFP']
    commands = [
        [compiler / 'armcc.exe', *common, '--c99', '-O0', '-I', source / 'stubs',
         '-I', root / 'Core/Inc', '-c', source / 'test_corexy.c', '-o', out / 'test_corexy.o'],
        [compiler / 'armasm.exe', *common, source / 'startup_test.s', '-o', out / 'startup_test.o'],
        [compiler / 'armlink.exe', '--cpu', 'Cortex-M4', '--ro-base', '0x0', '--rw-base',
         '0x20000000', '--entry', 'Reset_Handler', '--first', 'startup_test.o(RESET)',
         out / 'startup_test.o', out / 'test_corexy.o', '-o', out / 'test_corexy.axf'],
        [compiler / 'fromelf.exe', '--bin', '--output', out / 'test_corexy.bin', out / 'test_corexy.axf'],
    ]
    for command in commands:
        subprocess.run([str(p) for p in command], check=True, cwd=root)
    # Optional isolated package directory; otherwise use the active Python env.
    sys.path.insert(0, str(out / 'python'))
    from unicorn import Uc, UC_ARCH_ARM, UC_MODE_THUMB, UC_MODE_MCLASS, UC_HOOK_INTR
    from unicorn.arm_const import (UC_ARM_REG_R0, UC_ARM_REG_R1, UC_ARM_REG_SP,
                                   UC_ARM_REG_PC, UC_CPU_ARM_CORTEX_M4)
    image = (out / 'test_corexy.bin').read_bytes()
    cpu = Uc(UC_ARCH_ARM, UC_MODE_THUMB | UC_MODE_MCLASS)
    cpu.ctl_set_cpu_model(UC_CPU_ARM_CORTEX_M4)
    cpu.mem_map(0, 0x200000)
    cpu.mem_map(0x20000000, 0x100000)
    cpu.mem_write(0, image)
    sp, pc = struct.unpack_from('<II', image)
    cpu.reg_write(UC_ARM_REG_SP, sp)
    output = []
    ended = False
    exit_code = 0

    def words(address, count):
        return struct.unpack('<' + 'I' * count, bytes(cpu.mem_read(address, 4 * count)))

    def cstring(address):
        data = bytearray()
        while True:
            char = cpu.mem_read(address + len(data), 1)[0]
            if char == 0:
                return data.decode(errors='replace')
            data.append(char)

    def semihost(_cpu, number, _data):
        nonlocal ended, exit_code
        op = cpu.reg_read(UC_ARM_REG_R0)
        arg = cpu.reg_read(UC_ARM_REG_R1)
        address = cpu.reg_read(UC_ARM_REG_PC)
        if bytes(cpu.mem_read(address, 2)) != b'\xab\xbe':
            raise RuntimeError(f'Unexpected interrupt {number} at {address:#x}')
        result = 0
        if op == 0x01:  # SYS_OPEN: only :tt is allowed, never access host files.
            name, _mode, size = words(arg, 3)
            if bytes(cpu.mem_read(name, size)) != b':tt':
                raise RuntimeError('Test attempted to open a host file')
            result = 1
        elif op == 0x02:  # SYS_CLOSE
            pass
        elif op == 0x03:  # SYS_WRITEC
            output.append(bytes(cpu.mem_read(arg, 1)).decode())
        elif op == 0x04:  # SYS_WRITE0
            output.append(cstring(arg))
        elif op == 0x05:  # SYS_WRITE
            _fd, buffer, size = words(arg, 3)
            output.append(bytes(cpu.mem_read(buffer, size)).decode(errors='replace'))
        elif op == 0x09:  # SYS_ISTTY
            result = 1
        elif op in (0x10, 0x13):  # SYS_CLOCK, SYS_ERRNO
            result = 0
        elif op == 0x16:  # SYS_HEAPINFO
            block = words(arg, 1)[0]
            cpu.mem_write(block, struct.pack('<4I', 0x20080000, 0x200c0000,
                                            0x200ff000, 0x200c0000))
        elif op in (0x18, 0x20):  # SYS_EXIT / SYS_EXIT_EXTENDED
            if op == 0x20:
                reason, exit_code = words(arg, 2)
            else:
                reason = arg
            if reason != 0x20026:
                exit_code = 1
            ended = True
            cpu.emu_stop()
        else:
            raise RuntimeError(f'Unsupported semihost operation {op:#x}')
        cpu.reg_write(UC_ARM_REG_R0, result)
        cpu.reg_write(UC_ARM_REG_PC, (address + 2) | 1)

    cpu.hook_add(UC_HOOK_INTR, semihost)
    cpu.emu_start(pc, 0, count=20_000_000)
    text = ''.join(output)
    print(text, end='')
    if not ended or exit_code or 'PASS:' not in text or 'FAIL' in text:
        raise SystemExit('CoreXY regression tests failed or did not complete')


if __name__ == '__main__':
    main()
