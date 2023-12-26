#include "print.h"

#define fpf(A...) fprintf(out, A)

void printOp(Op *op) {
  switch (op->tag) {
  case CAT:
    printOp(op->vCAT.l);
    fpf("..");
    printOp(op->vCAT.r);
    break;
  case STR:
    fpf("'%s'", op->vSTR);
    break;
  case SLICE:
    fpf("(slice %d)", op->vSLICE);
    break;
  case SWAP:
    fpf("(swap %d ", op->vCALL.listIndex);
    printOp(op->vCALL.op);
    fpf(")");
    break;
  }
}

void printLists() {
  for (u32 i = 0; i < topLen; i++) {
    if (i > 0)
      fpf("\n");
    fpf("==== LIST %d ====\n", i);
    fpf("\n");
    for (u32 j = 0; j < midLens[i]; j++) {
      NamedOp namedOp = opLists[i][j];
      char *name = namedOp.name;
      Op *op = namedOp.op;
      fpf("%s: ", name);
      printOp(op);
      fpf("\n");
    }
  }
}
