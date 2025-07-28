import os
import sys
from pathlib import Path

from PIL import Image


def binary_string_to_hex_list(binary_str):
    if not binary_str:
        return []

    if len(binary_str) % 8 != 0:
        raise ValueError(f"输入二进制字符串的长度 ({len(binary_str)}) 必须是8的倍数。")

    hex_list = []
    for i in range(0, len(binary_str), 8):
        byte_chunk = binary_str[i : i + 8]

        if not all(c in "01" for c in byte_chunk):
            raise ValueError(f"在位置 {i} 发现无效的二进制字符块: '{byte_chunk}'")

        decimal_value = int(byte_chunk, 2)

        hex_value = (
            f"0x{decimal_value:02x}"
        )
        hex_list.append(hex_value)

    return hex_list


show_bytes = False
img_dir = sys.argv[1]
for index, fn in enumerate(os.listdir(img_dir)):
    path = Path(img_dir) / fn
    if fn == ".DS_Store":
        continue
    img = Image.open(path)
    # print(dir(img))
    # print(img.width, img.height)

    width = img.width // 7
    height = img.height // 7

    result = []
    for x in range(width):
        for y in range(height):
            p = img.getpixel((x * 7 + 3, y * 7 + 3))
            if p[3] == 255:
                result.append("1")
            else:
                result.append("0")

    n = len(result) % 8
    if n > 0:
        result.append("0" * (8 - n))
    hex_output = binary_string_to_hex_list("".join(result))
    if not show_bytes:
        print(len(hex_output))
        show_bytes = True

    print("[" + ", ".join(hex_output) + f"], # {fn}")
