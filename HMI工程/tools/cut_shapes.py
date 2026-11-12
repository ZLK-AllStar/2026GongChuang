# -*- coding: utf-8 -*-
"""
从 6 张屏幕截图中切出 7 种颜色 (红橙黄绿青蓝紫) 的几何体。

每张截图为浏览器全屏渲染的 SVG 页面, 页面内 4+3 排列 7 个同形状不同颜色的几何体。
处理流程:
  1. RGB -> HSV, 取高饱和度高亮度掩膜 (几何体区域, 排除白色背景/灰色浏览器边框)
  2. 掩膜降采样后做连通域标记, 得到 7 个外接框
  3. 按行(上4下3)与 x 坐标排序 → 红橙黄绿 | 青蓝紫, 并用平均色调校验
  4. 裁剪后居中贴到 400x400 白底画布, 保存为 images/<前缀>-<颜色>.png
输出命名与工程现有 RT-*.png (正四面体) 约定一致:
  TP=三棱柱 PP=五棱柱 HP=六棱柱 CY=圆柱形 CB=长方体 CU=正方体
"""
import os
import numpy as np
from PIL import Image
from collections import deque

PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
IMG_DIR = os.path.join(PROJ, "images")
OUT_SIZE = 400          # 输出画布边长
MAX_FIT = 380           # 形状最大内接尺寸

# 截图文件名时间戳 -> (输出前缀, 形状中文名)
SHAPES = [
    ("142818", "RT", "正四面体"),
    ("135138", "TP", "三棱柱"),
    ("135146", "PP", "五棱柱"),
    ("134845", "HP", "六棱柱"),
    ("135152", "CY", "圆柱形"),
    ("135157", "CB", "长方体"),
    ("135204", "CU", "正方体"),
]

# 上排4个 + 下排3个 的颜色顺序 (与工程 RT-*/指令码 1~7 一致)
ROW_COLORS = ["RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLUE", "PURPLE"]
COLOR_CN   = {"RED": "红", "ORANGE": "橙", "YELLOW": "黄", "GREEN": "绿",
              "CYAN": "青", "BLUE": "蓝", "PURPLE": "紫"}

# 各颜色的标准色调(度), 用于校验排序结果
REF_HUE = {"RED": 0, "ORANGE": 30, "YELLOW": 55, "GREEN": 120,
           "CYAN": 180, "BLUE": 225, "PURPLE": 285}


def rgb_to_hue_deg(r, g, b):
    """单个 RGB 像素色调 (0~360)。"""
    mx, mn = max(r, g, b), min(r, g, b)
    if mx == mn:
        return 0.0
    d = mx - mn
    if mx == r:
        h = (g - b) / d % 6
    elif mx == g:
        h = (b - r) / d + 2
    else:
        h = (r - g) / d + 4
    return h * 60


def label_components(mask):
    """4 邻域连通域标记, 返回 [(面积, x0,y0,x1,y1), ...] (降采样坐标系)。"""
    h, w = mask.shape
    labels = np.zeros((h, w), dtype=np.int32)
    boxes = []
    cur = 0
    for sy in range(h):
        for sx in range(w):
            if mask[sy, sx] and labels[sy, sx] == 0:
                cur += 1
                q = deque([(sy, sx)])
                labels[sy, sx] = cur
                x0 = x1 = sx
                y0 = y1 = sy
                area = 0
                while q:
                    y, x = q.popleft()
                    area += 1
                    x0, x1 = min(x0, x), max(x1, x)
                    y0, y1 = min(y0, y), max(y1, y)
                    for ny, nx in ((y-1, x), (y+1, x), (y, x-1), (y, x+1)):
                        if 0 <= ny < h and 0 <= nx < w and mask[ny, nx] and labels[ny, nx] == 0:
                            labels[ny, nx] = cur
                            q.append((ny, nx))
                boxes.append((area, x0, y0, x1, y1))
    return boxes


def cut_one(path, prefix, shape_cn):
    im = Image.open(path).convert("RGB")
    W, H = im.size
    arr = np.asarray(im, dtype=np.uint8)

    # ---- 1. 高饱和度掩膜 (几何体) ----
    mx = arr.max(axis=2).astype(np.int16)
    mn = arr.min(axis=2).astype(np.int16)
    sat = mx - mn                       # 0~255 简化饱和度
    mask = (sat > 60) & (mx > 60)

    # ---- 2. 降采样 1/4 找连通域 ----
    small = mask[::4, ::4]
    boxes = label_components(small)
    boxes = [b for b in boxes if b[0] > 200]        # 去掉噪点
    boxes.sort(key=lambda b: -b[0])
    if len(boxes) != 7:
        raise RuntimeError("%s: 期望7个几何体, 实际%d个" % (os.path.basename(path), len(boxes)))
    boxes = boxes[:7]

    # 上排(y质心小)4个 = 红橙黄绿, 下排3个 = 青蓝紫
    def cy(b):
        return (b[2] + b[4]) / 2.0
    mid = (min(cy(b) for b in boxes) + max(cy(b) for b in boxes)) / 2.0
    top = sorted([b for b in boxes if cy(b) < mid], key=lambda b: b[1])
    bot = sorted([b for b in boxes if cy(b) >= mid], key=lambda b: b[1])
    ordered = top + bot
    if len(top) != 4 or len(bot) != 3:
        raise RuntimeError("%s: 行分割异常 top=%d bot=%d" % (os.path.basename(path), len(top), len(bot)))

    # ---- 3. 色调校验 ----
    report = []
    for b, color in zip(ordered, ROW_COLORS):
        _, x0, y0, x1, y1 = b
        region = arr[y0*4:(y1+1)*4, x0*4:(x1+1)*4].reshape(-1, 3)
        sel = region[region.max(axis=1) - region.min(axis=1) > 60]
        r, g, bl = sel.mean(axis=0)
        hue = rgb_to_hue_deg(r, g, bl)
        diff = min(abs(hue - REF_HUE[color]), 360 - abs(hue - REF_HUE[color]))
        if diff > 30:
            raise RuntimeError("%s: %s 色调校验失败 hue=%.0f" % (os.path.basename(path), color, hue))
        report.append("%s(hue%.0f)" % (color, hue))

        # ---- 4. 原分辨率精确 bbox + 裁剪 + 居中放置 ----
        sub = mask[y0*4:(y1+1)*4, x0*4:(x1+1)*4]
        ys, xs = np.where(sub)
        px0, px1 = x0*4 + xs.min(), x0*4 + xs.max() + 1
        py0, py1 = y0*4 + ys.min(), y0*4 + ys.max() + 1
        pad = 4
        crop = im.crop((max(px0-pad, 0), max(py0-pad, 0),
                        min(px1+pad, W), min(py1+pad, H)))

        # 等比缩放到 MAX_FIT 内
        cw, ch = crop.size
        scale = min(MAX_FIT / cw, MAX_FIT / ch, 1.0)
        if scale < 1.0:
            crop = crop.resize((int(cw*scale), int(ch*scale)), Image.LANCZOS)
        cw, ch = crop.size

        canvas = Image.new("RGBA", (OUT_SIZE, OUT_SIZE), (255, 255, 255, 255))
        canvas.paste(crop, ((OUT_SIZE - cw)//2, (OUT_SIZE - ch)//2))
        out = os.path.join(IMG_DIR, "%s-%s.png" % (prefix, color))
        canvas.save(out)
    return report


def main():
    files = sorted(f for f in os.listdir(IMG_DIR) if "屏幕截图" in f)
    for stamp, prefix, shape_cn in SHAPES:
        match = [f for f in files if stamp in f]
        if not match:
            raise RuntimeError("找不到截图: %s" % stamp)
        report = cut_one(os.path.join(IMG_DIR, match[0]), prefix, shape_cn)
        print("%s(%s): %s" % (shape_cn, prefix, " ".join(report)))
    print("完成: %d 形状 x 7 颜色 = %d 张 ->" % (len(SHAPES), len(SHAPES)*7), IMG_DIR)


if __name__ == "__main__":
    main()
