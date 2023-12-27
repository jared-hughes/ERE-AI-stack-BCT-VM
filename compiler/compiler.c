#include "compiler.h"

int compiler_main(int argc, char *argv[]) {
  initFiles(argv[1]);
  MultiOpList multiOpList = parseMultiOpList();
  printLists(multiOpList);
  IntList bitList = writeBitsForLists(multiOpList);
  IntList chars = charsForBits(bitList);
  FILE *out = fopen("bin/out.bin", "wb");
  for (u32 i = 0; i < chars.len; i++) {
    fputc(chars.elems[i], out);
  }
  fclose(out);
}
