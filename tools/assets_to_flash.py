#!/usr/bin/env python3
"""Convert TEST/images geometry PNGs to RGB565 data for MCU internal Flash.

The generated C object is const, so Keil places it in the Flash read-only
region.  The binary file is useful for an external programming step or asset
inspection; the firmware consumes the generated C file.
"""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

from PIL import Image

SHAPES = ("RT", "TP", "PP", "HP", "CY", "CB", "CU", "SP")
COLORS = ("RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLUE", "PURPLE", "GRAY", "BLACK")


def rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def convert(path: Path, size: int) -> list[int]:
    image = Image.open(path).convert("RGBA")
    image.thumbnail((size, size), Image.Resampling.LANCZOS)
    canvas = Image.new("RGBA", (size, size), (0, 0, 0, 255))
    canvas.alpha_composite(image, ((size - image.width) // 2, (size - image.height) // 2))
    pixels = []
    for r, g, b, a in canvas.getdata():
        if a < 16:
            r = g = b = 0
        pixels.append(rgb565(r, g, b))
    return pixels


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, default=Path("TEST/images"))
    parser.add_argument("--output-bin", type=Path, default=Path("Core/Assets/geometry_assets.bin"))
    parser.add_argument("--output-c", type=Path, default=Path("Core/Src/geometry_assets.c"))
    parser.add_argument("--output-h", type=Path, default=Path("Core/Inc/geometry_assets.h"))
    parser.add_argument("--size", type=int, default=20)
    args = parser.parse_args()
    if args.size <= 0 or args.size > 128:
        raise SystemExit("--size must be between 1 and 128")

    args.output_bin.parent.mkdir(parents=True, exist_ok=True)
    args.output_c.parent.mkdir(parents=True, exist_ok=True)
    args.output_h.parent.mkdir(parents=True, exist_ok=True)
    all_pixels: list[int] = []
    rows: list[str] = []
    for shape in SHAPES:
        for color in COLORS:
            path = args.input / f"{shape}-{color}.png"
            if not path.exists():
                raise SystemExit(f"missing asset: {path}")
            all_pixels.extend(convert(path, args.size))
            rows.append(f"  {{ {len(rows):3d}U, {SHAPES.index(shape):2d}U, {COLORS.index(color):2d}U }},")

    with args.output_bin.open("wb") as output:
        for pixel in all_pixels:
            output.write(struct.pack(">H", pixel))

    with args.output_h.open("w", encoding="ascii") as output:
        output.write("#ifndef __GEOMETRY_ASSETS_H__\n#define __GEOMETRY_ASSETS_H__\n")
        output.write("#include <stdint.h>\n")
        output.write(f"#define GEOMETRY_ASSET_SIZE {args.size}U\n")
        output.write(f"#define GEOMETRY_ASSET_COUNT {len(rows)}U\n")
        output.write("extern const uint16_t GeometryAssets[GEOMETRY_ASSET_COUNT][GEOMETRY_ASSET_SIZE * GEOMETRY_ASSET_SIZE];\n")
        output.write("uint16_t GeometryAssetIndex(uint8_t shape, uint8_t color);\n#endif\n")

    with args.output_c.open("w", encoding="ascii") as output:
        output.write('#include "geometry_assets.h"\n\n')
        output.write("const uint16_t GeometryAssets[GEOMETRY_ASSET_COUNT][GEOMETRY_ASSET_SIZE * GEOMETRY_ASSET_SIZE] = {\n")
        for index in range(len(rows)):
            start = index * args.size * args.size
            values = all_pixels[start:start + args.size * args.size]
            output.write("  {\n")
            for offset in range(0, len(values), 16):
                output.write("    " + ", ".join(f"0x{v:04X}U" for v in values[offset:offset + 16]) + ",\n")
            output.write("  },\n")
        output.write("};\n\n")
        output.write("uint16_t GeometryAssetIndex(uint8_t shape, uint8_t color)\n{\n")
        output.write("  return (uint16_t)(shape * 9U + color);\n}\n")


if __name__ == "__main__":
    main()
