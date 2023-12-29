#!/usr/bin/env python3

import sys

s = sys.argv[1] if len(sys.argv) > 1 else input()
out = ""
for c in s:
    if c == "0":
        out += "10"
    elif c == "1":
        out += "11"
print("0" + out + "00")
