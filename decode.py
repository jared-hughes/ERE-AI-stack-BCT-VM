#!/usr/bin/env python3

import sys
import re

s = sys.argv[1] if len(sys.argv) > 1 else input()
match = re.match(r"^((1[01])*)0*$", s)
if match:
    print(match.group(1)[1::2])
else:
    print("Didn't match pattern.")
print("Raw:", s)
