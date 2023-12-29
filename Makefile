.PHONY: all
all: bin/raw-interp bin/interp bin/compiler

bin/raw-interp: $(shell find raw)
	mkdir -p bin
	gcc raw/raw-interp.c -o bin/raw-interp

bin/interp bin/compiler: main.c main.h $(shell find deobfuscated compiler)
	mkdir -p bin
	gcc -g -DINTERP main.c {compiler,deobfuscated}/*.c -Icompiler -Ideobfuscated -o bin/interp
	gcc -g main.c {compiler,deobfuscated}/*.c -Icompiler -Ideobfuscated -o bin/compiler

names = 01_skip_first 02_skip_last 03_repeat 04_hello 05_sort 06_increment 07_decrement 08_zeros_ones_same 09_truth_machine
namesLUA = 
BINSnamesBIN = $(addprefix bins/,$(addsuffix .bin,$(names)))
# SOURCESnamesLUA = $(addprefix sources/,$(addsuffix .bin,$(names)))

bins: $(BINSnamesBIN)

.PHONY: bins-len
bins-len: bins
	wc -c bins/*

$(BINSnamesBIN): bins/%.bin: sources/%.lua bin/compiler
	mkdir -p bins
	./bin/compiler $<
	cp bin/out.bin $@

SUBMIT_COMMON = bins encode.py decode.py run_bin.sh

.PHONY: submit
submit: $(SUBMIT_COMMON) bin/raw-interp
	mv bin/interp bin/safe-interp
	mv bin/raw-interp bin/interp
	tar --create --file=submit.tar $(SUBMIT_COMMON) bin/interp
	mv bin/interp bin/raw-interp
	mv bin/safe-interp bin/interp
