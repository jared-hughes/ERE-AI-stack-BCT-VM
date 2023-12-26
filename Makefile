bin/raw-interp: $(shell find raw)
	mkdir -p bin
	gcc raw/raw-interp.c -o bin/raw-interp

bin/interp: $(shell find deobfuscated)
	mkdir -p bin
	gcc deobfuscated/interp.c -o bin/interp
