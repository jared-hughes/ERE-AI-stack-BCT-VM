bin:
	mkdir -p bin

bin/raw-interp: bin
	gcc raw/raw-interp.c -o bin/raw-interp
