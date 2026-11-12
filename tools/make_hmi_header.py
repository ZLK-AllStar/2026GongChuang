"""Render the Frm02 record-list header strip used as the first list row.

The SDK list drops the text style on the first row created after clearOptions(),
so the record list uses an image-only header row instead of a text row. The
banner is drawn at its natural size, therefore its width matches the list
(971 px) and its height matches the default row height (~41 px).

Usage: python -X utf8 tools/make_hmi_header.py [--project <HMI project dir>]
"""
from pathlib import Path
import argparse

from PIL import Image, ImageDraw, ImageFont

WIDTH, HEIGHT = 971, 41
BORDER = (0xC0, 0xC0, 0xC0)
TEXT = (0x33, 0x33, 0x33)
MUTED = (0x88, 0x88, 0x88)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--project", type=Path,
                        default=Path("D:/BaiduNetdiskDownload/ApusIDE2_v20250930/"
                                     "ApusIDE2_v20250930/workspace/TEST"))
    args = parser.parse_args()
    font_path = args.project / "fnt/wenquanyiweimihei.ttf"
    font = ImageFont.truetype(str(font_path), 18)
    small = ImageFont.truetype(str(font_path), 16)

    image = Image.new("RGB", (WIDTH, HEIGHT), (255, 255, 255))
    draw = ImageDraw.Draw(image)
    draw.line([(0, HEIGHT - 1), (WIDTH - 1, HEIGHT - 1)], fill=BORDER)

    def centred(text, x, font_used, colour):
        box = draw.textbbox((0, 0), text, font=font_used)
        y = (HEIGHT - (box[3] - box[1])) // 2 - box[1]
        draw.text((x, y), text, font=font_used, fill=colour)

    # Column origins match the record rows (record text starts at page x=451,
    # 490 and 548; the list adds a 41 px left offset to this banner).
    centred("形状预览", 232, small, MUTED)
    centred("ID", 410, font, TEXT)
    centred("指令", 449, font, TEXT)
    centred("形状-颜色", 507, font, TEXT)
    centred("同类总数", 598, font, TEXT)

    out = args.project / "images/HDR-RECORDS.png"
    image.save(out, optimize=True)
    print("wrote", out, image.size)


if __name__ == "__main__":
    main()