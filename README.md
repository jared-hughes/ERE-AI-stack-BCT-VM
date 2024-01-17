Encoding is 0→10 and 1→11, with an extra 0 prepended and 00 appended.

Decoding trims leading/trailing 0s and does that substitution backwards.

## Scripts

Development:

```bash
./run.sh sources/08_zeros_ones_same.lua 110
```

Testing manually: (`make bins` to get the binary files)

```bash
./run_bin.sh bins/08_zeros_ones_same.bin 0101
```

Automated testing:

```bash
./test.py tests/*
```

Submit/release

```bash
make submit
```

## Byte counts

```sh
$ make bins-len
wc -c bins/*
 5 bins/01_skip_first.bin
 7 bins/02_skip_last.bin
 6 bins/03_repeat.bin
19 bins/04_hello.bin
 9 bins/05_sort.bin
13 bins/06_increment.bin
10 bins/07_decrement.bin
25 bins/08_zeros_ones_same.bin
 3 bins/09_truth_machine.bin
97 total
```
