#!/usr/bin/env python3

import subprocess

import sys
import re

if len(sys.argv) < 2:
    print("Help:")
    print("./test.py tests/01_skip_first.lua")
    sys.exit(1)


def test(tests_file):
    print("Testing", tests_file)
    source = tests_file.replace("tests/", "sources/")

    with open(tests_file) as f:
        lines = f.readlines()

    for line in lines:
        input, exp = line.strip().split(":")
        input = input.strip()
        exp = exp.strip()
        assert re.match(r"^[01]*$", input)
        assert re.match(r"^[01]*$", exp)
        r = subprocess.run(["./run.sh", source, input], capture_output=True)
        got = r.stdout.strip()
        exp = exp.encode().strip()
        if exp == got:
            print("PASS")
        else:
            print("FAIL exp=", exp, ", got=", got, sep="")


for tests_file in sys.argv[1:]:
    test(tests_file)
