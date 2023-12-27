#include "print.h"

FILE *in;
FILE *out;
FILE *compilerLogOut;

void initFiles(char *inFileName) {
  if (!inFileName) {
    epf("Missing input file");
    exit(1);
  }
  in = fopen(inFileName, "rb");
  compilerLogOut = fopen("logs/compiler.log", "w");
  out = fopen("logs/interp.log", "w");
}

void cprintOp(Op *op) {
  FILE *tmp = out;
  out = compilerLogOut;
  printOp(op);
  out = tmp;
}

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
    fpf("(swap %d ", op->vSWAP.listIndex);
    printOp(op->vSWAP.op);
    fpf(")");
    break;
  }
}

char *nameTag(OpTag tag) {
  switch (tag) {
  case CAT:
    return "CAT";
  case STR:
    return "STR";
  case SLICE:
    return "SLICE";
  case SWAP:
    return "SWAP";
  }
}

void printNamedOp(NamedOp namedOp) {
  fpf("%s: ", namedOp.name);
  printOp(namedOp.op);
}

void printList(OpList opList) {
  for (u32 j = 0; j < opList.len; j++) {
    printNamedOp(opList.ops[j]);
    fpf("\n");
  }
  fpf("\n");
}

void printLists(MultiOpList multiOpList) {
  for (u32 i = 0; i < multiOpList.len; i++) {
    fpf("=== LIST %d ===\n", i);
    fpf("\n");
    printList(multiOpList.opLists[i]);
  }
}

u64 gcd(u64 a, u64 b) {
  while (b != 0) {
    a = a % b;
    if (a == 0)
      return b;
    b = b % a;
  }
  return a;
}

/** Prints simplified fraction prop/(1<<32) */
void printProp(u32 prop) {
  u64 a = prop;
  u64 b = ((u64)1) << 32;
  u64 g = gcd(a, b);
  char buf[50];
  snprintf(buf, 49, "0x%llx", a / g);
  int len = strlen(buf);
  for (int i = 0; i < 11 - len; i++) {
    fpf(" ");
  }
  fpf("%s/0x%-9llx", buf, b / g);
}
