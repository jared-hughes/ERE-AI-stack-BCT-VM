all: bin/raw-interp bin/interp bin/compiler

bin/raw-interp: $(shell find raw)
	mkdir -p bin
	gcc raw/raw-interp.c -o bin/raw-interp

bin/interp bin/compiler: main.c main.h $(shell find deobfuscated compiler)
	mkdir -p bin
	gcc -DINTERP main.c {compiler,deobfuscated}/*.c -Icompiler -Ideobfuscated -o bin/interp
	gcc main.c {compiler,deobfuscated}/*.c -Icompiler -Ideobfuscated -o bin/compiler
