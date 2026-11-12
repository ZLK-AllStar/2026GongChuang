"""Generate (and optionally send) MCU HMI protocol frames for panel testing.

The panel receives on COM2 at 115200 8N1. Two encodings exist:
  * preview: one byte 1..90 (legacy path, no ID, never enters production history)
  * record : 22 bytes F0 + 20 nibble bytes (0x80|n) + F1, CRC16/CCITT-FALSE

Examples:
    python -X utf8 tools/hmi_frame_test.py
    python -X utf8 tools/hmi_frame_test.py --record 19 3 --seq 1
    python -X utf8 tools/hmi_frame_test.py --port COM7 --reset --record 19 3
"""
from pathlib import Path
import argparse
import sys


def crc16(data):
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
    return crc


def frame(record_type, sequence, code, ident):
    """Return the 22-byte wire frame plus the 10 decoded payload bytes."""
    data = bytearray([1, record_type])
    data += sequence.to_bytes(4, "little")
    data += bytes([code, ident])
    data += crc16(data).to_bytes(2, "little")
    wire = bytearray([0xF0])
    for byte in data:
        wire += bytes([0x80 | (byte >> 4), 0x80 | (byte & 0x0F)])
    wire += b"\xF1"
    assert len(wire) == 22
    return bytes(wire), bytes(data)


def show(label, wire):
    print(f"{label:<34} {wire.hex(' ').upper()}")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--record", nargs=2, type=int, metavar=("CODE", "ID"),
                        help="type=1 production record, CODE 1..90, ID 1..6")
    parser.add_argument("--seq", type=int, default=None, help="record sequence (default: 1)")
    parser.add_argument("--reset", action="store_true", help="type=2 new session / clear history")
    parser.add_argument("--preview", type=int, metavar="CODE",
                        help="legacy single byte preview, CODE 1..90")
    parser.add_argument("--port", help="send over this serial port, e.g. COM7")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--gap", type=float, default=0.3, help="seconds between frames when sending")
    args = parser.parse_args()

    payloads = []
    if args.reset:
        wire, data = frame(2, 0, 0, 0)
        show("new session (type=2)", wire)
        payloads.append(wire)
    if args.record:
        code, ident = args.record
        if not 1 <= code <= 90 or not 1 <= ident <= 6:
            return print("ERROR: CODE 1..90, ID 1..6") or 1
        sequence = 1 if args.seq is None else args.seq
        wire, data = frame(1, sequence, code, ident)
        show(f"record seq={sequence} code={code} id={ident}", wire)
        payloads.append(wire)
    if args.preview is not None:
        code = args.preview
        if not 1 <= code <= 90:
            return print("ERROR: CODE 1..90") or 1
        show(f"preview byte {code}", bytes([code]))
        payloads.append(bytes([code]))

    if not payloads:
        print("standard test set (send one frame at a time, hex mode):")
        print()
        show("1) new session, clears history", frame(2, 0, 0, 0)[0])
        show("2) record seq=1 code=19 id=3", frame(1, 1, 19, 3)[0])
        show("3) record seq=2 code=1 id=1", frame(1, 2, 1, 1)[0])
        show("4) record seq=3 code=72 id=6", frame(1, 3, 72, 6)[0])
        show("5) white tetrahedron code=73 id=1", frame(1, 4, 73, 1)[0])
        show("6) white sphere code=80 id=2", frame(1, 5, 80, 2)[0])
        show("7) preview white sphere (0x50)", bytes([80]))
        show("8) red cone code=81 id=3", frame(1, 6, 81, 3)[0])
        show("9) white cone code=90 id=4", frame(1, 7, 90, 4)[0])
        print()
        print("expected: '串口' shows the record text, Frm02 list grows one row per record.")
        print("send with: python -X utf8 tools/hmi_frame_test.py --port COM7 --reset --record 19 3")
        return 0

    if args.port:
        try:
            import serial
        except ImportError:
            return print("ERROR: pyserial missing: pip install pyserial") or 1
        import time
        with serial.Serial(args.port, args.baud, bytesize=8, parity="N", stopbits=1,
                           timeout=0.2, write_timeout=1) as port:
            for wire in payloads:
                port.write(wire)
                port.flush()
                print("sent", len(wire), "bytes")
                time.sleep(args.gap)
        return 0

    print()
    print("(no --port: nothing sent; copy the hex above into a serial assistant, hex mode)")
    return 0


if __name__ == "__main__":
    sys.exit(main())