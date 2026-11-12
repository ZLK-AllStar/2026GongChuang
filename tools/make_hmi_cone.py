"""Render the ten cone icons and their appended HMI code mapping (81..90).

Use the existing 400x400 icon style. White uses an opaque black background;
the other nine colors use white. Supersampling keeps the silhouette smooth.
"""
from pathlib import Path
import argparse
import csv
import math

import numpy as np
from PIL import Image, ImageDraw, ImageFont

DEFAULT_HMI = Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/"
                   "ApusIDE2_v20250930/workspace/TEST")
COLORS = [
    ("RED", "红", (255, 0, 0)), ("ORANGE", "橙", (255, 145, 0)),
    ("YELLOW", "黄", (255, 220, 0)), ("GREEN", "绿", (0, 210, 35)),
    ("CYAN", "青", (0, 210, 220)), ("BLUE", "蓝", (0, 90, 255)),
    ("PURPLE", "紫", (160, 0, 255)), ("GRAY", "灰", (155, 155, 155)),
    ("BLACK", "黑", (35, 35, 35)), ("WHITE", "白", (245, 245, 245)),
]


def cone_mask(scale):
    # The sloped sides are tangent to the base ellipse, avoiding a shoulder.
    cx, cy, rx, ry, tip = 200, 312, 163, 36, 28
    angle = math.asin(-ry / (cy - tip))
    arc = [(cx + rx * math.cos(a), cy + ry * math.sin(a))
           for a in np.linspace(angle, math.pi - angle, 361)]
    polygon = [(cx, tip)] + arc
    mask = Image.new("L", (400 * scale, 400 * scale))
    ImageDraw.Draw(mask).polygon([(round(x * scale), round(y * scale))
                                 for x, y in polygon], fill=255)
    return mask


def render(rgb, white=False):
    scale = 3
    size = 400 * scale
    y, x = np.mgrid[0:size, 0:size].astype(np.float32) / scale
    half_width = np.maximum(1, (y - 28) * 163 / (312 - 28))
    u = np.clip((x - 200) / half_width, -1, 1)
    shade = 0.60 + 0.38 * np.cos((u + 0.25) * 1.5)
    shine = 0.18 * np.exp(-((u + 0.35) / 0.22) ** 2)
    base = np.asarray(rgb, dtype=np.float32)
    pixels = base * shade[:, :, None]
    pixels += (255 - pixels) * shine[:, :, None]
    foreground = Image.fromarray(np.clip(pixels, 0, 255).astype(np.uint8))
    background = Image.new("RGB", (size, size), "black" if white else "white")
    background.paste(foreground, (0, 0), cone_mask(scale))
    return background.resize((400, 400), Image.Resampling.LANCZOS)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project", type=Path, default=DEFAULT_HMI)
    args = parser.parse_args()
    font = ImageFont.truetype(str(args.project / "fnt/wenquanyiweimihei.ttf"), 20)
    sheet = Image.new("RGB", (1250, 580), "#161b22")
    draw = ImageDraw.Draw(sheet)
    rows = []
    for index, (color, name, rgb) in enumerate(COLORS):
        code = 81 + index
        image = render(rgb, color == "WHITE")
        relative = f"images/CO-{color}.png"
        image.save(args.project / relative, optimize=True)
        rows.append([code, f"0x{code:02X}", 8, "圆锥", index, name, relative])
        x, y = index % 5 * 250, index // 5 * 290
        sheet.paste(image.resize((240, 240), Image.Resampling.LANCZOS), (x + 5, y + 5))
        draw.text((x + 10, y + 252), f"{code} / {code:02X}  圆锥-{name}", font=font, fill="white")
        print(code, f"0x{code:02X}", name, relative)
    with (args.project / "圆锥系列映射.csv").open("w", encoding="utf-8-sig", newline="") as stream:
        writer = csv.writer(stream)
        writer.writerow(["code_dec", "code_hex", "shape_index", "shape_name",
                         "color_index", "color_name", "image_path"])
        writer.writerows(rows)
    sheet.save(args.project / "圆锥系列预览.png", optimize=True)


if __name__ == "__main__":
    main()
