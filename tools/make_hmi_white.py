"""Derive black-background white HMI assets from the existing gray geometry.

Preserves the source canvas, silhouette, faces and lighting. No original PNG
is changed. Also writes a UTF-8 CSV mapping and a contact sheet for review.
"""
from pathlib import Path
import argparse
import csv

from PIL import Image, ImageDraw, ImageFilter, ImageFont

DEFAULT_HMI = Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/"
                   "ApusIDE2_v20250930/workspace/TEST")
SHAPES = [("RT", "正四面体"), ("TP", "三棱柱"), ("PP", "五棱柱"),
          ("HP", "六棱柱"), ("CY", "圆柱形"), ("CB", "长方体"),
          ("CU", "正方体"), ("SP", "球体")]


def white_asset(source):
    gray = source.convert("L")
    # The local interior tone recovers edge coverage against the old white
    # background, so antialiased edges do not acquire a white square fringe.
    interior = gray.filter(ImageFilter.MinFilter(3))
    edge = gray.point(lambda value: 255 if value == 255 else 0).filter(ImageFilter.MaxFilter(3))
    pixels = []
    for value, solid, boundary in zip(gray.getdata(), interior.getdata(), edge.getdata()):
        if value == 255:
            pixels.append(0)
        elif boundary:
            coverage = min(1.0, (255 - value) / max(1, 255 - solid))
            pixels.append(round(min(255, 100 + 0.85 * solid) * coverage))
        else:
            pixels.append(round(min(255, 100 + 0.85 * value)))
    result = Image.new("L", gray.size)
    result.putdata(pixels)
    return result.convert("RGB")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project", type=Path, default=DEFAULT_HMI)
    args = parser.parse_args()
    font = ImageFont.truetype(str(args.project / "fnt/wenquanyiweimihei.ttf"), 20)
    sheet = Image.new("RGB", (1000, 580), "#161b22")
    draw = ImageDraw.Draw(sheet)
    rows = []
    for shape, (prefix, name) in enumerate(SHAPES):
        source = args.project / f"images/{prefix}-GRAY.png"
        with Image.open(source) as original:
            image = white_asset(original)
        relative = f"images/{prefix}-WHITE.png"
        image.save(args.project / relative, optimize=True)
        code = 73 + shape
        rows.append([code, f"0x{code:02X}", shape, name, 9, "白", relative])
        x, y = (shape % 4) * 250, (shape // 4) * 290
        sheet.paste(image.resize((240, 240), Image.Resampling.LANCZOS), (x + 5, y + 5))
        draw.text((x + 10, y + 252), f"{code} / {code:02X}  {name}", font=font, fill="white")
        print(code, f"0x{code:02X}", name, image.size, relative)
    with (args.project / "白色几何体映射.csv").open("w", encoding="utf-8-sig", newline="") as stream:
        writer = csv.writer(stream)
        writer.writerow(["code_dec", "code_hex", "shape_index", "shape_name",
                         "color_index", "color_name", "image_path"])
        writer.writerows(rows)
    sheet.save(args.project / "白色几何体预览.png", optimize=True)


if __name__ == "__main__":
    main()
