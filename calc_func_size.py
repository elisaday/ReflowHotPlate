#!/usr/bin/env python

import sys

map_file = sys.argv[1]

seg_found = False
start = False
last_addr = 0
last_name = "__start__"
lines = open(map_file).readlines()
for line in lines:
    if line.startswith("CSEG"):
        seg_found = True
    elif seg_found:
        if line.startswith("C:"):
            start = True
            items = line.split()
            addr = int(f"0x{items[1]}", 16)
            name = f"{items[3]}\t{items[2]}"
            print(f"{last_name}\t{addr - last_addr}")
            last_addr = addr
            last_name = name
        elif start:
            break
