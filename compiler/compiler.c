#include "compiler.h"

int compiler_main(int argc, char *argv[]) {
  initFiles(argv[1]);
  MultiOpList multiOpList = parseMultiOpList();
  printLists(multiOpList);
}
