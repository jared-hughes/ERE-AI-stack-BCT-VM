#!/bin/bash

set -euo pipefail

if (( $# < 2 )); then
  echo "Help:"
  echo "./run.sh bins/01_skip_first.bin 110001"
  exit 1
fi

./bin/interp "$1" "$(./encode.py "$2")" | ./decode.py
