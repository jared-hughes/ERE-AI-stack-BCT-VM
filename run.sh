#!/bin/bash

set -euo pipefail

if (( $# < 2 )); then
  echo "Help:"
  echo "./run.sh sources/mix.lua 110001"
fi

make all > /dev/null

./bin/compiler "$1"

./bin/interp bin/out.bin "$(./encode.py "$2")" | ./decode.py
