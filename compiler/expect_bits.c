#include "expect_bits.h"

void writeBit(u32 b) { cpf("Wrote bit %d\n", b); }

u32 charToBit(char c) {
  if (c == '1') {
    return 1;
  } else if (c == '0') {
    return 0;
  } else {
    epf("Invalid char %c (%02x)", c, c);
    exit(1);
  }
}

void writeBitsForNumber(u32 x) {
  cpf("Writing u32 %d.\n", x);
  u32 l2 = 0;
  while (x >> l2) {
    writeBit(1);
    l2++;
  }
  writeBit(0);
  cpf("----\n");
  while (l2--) {
    writeBit((x >> l2) & 1);
  }
  cpf("========\n");
}

void writeBitsForString(char *str) {
  cpf("Writing string '%s'\n", str);
  u32 len = strlen(str);
  writeBitsForNumber(len);
  for (int i = 0; i < len; i++) {
    writeBit(charToBit(str[i]));
  }
}

void writeBitsForOp(Op *op) {
  cpf("Making op ");
  cprintOp(op);
  cpf("\n");
  writeBit(op->tag >> 1 & 1);
  writeBit(op->tag & 1);
  switch (op->tag) {
  case CAT:
    writeBitsForOp(op->vCAT.l);
    writeBitsForOp(op->vCAT.r);
    break;
  case STR:
    writeBitsForString(op->vSTR);
    break;
  case SLICE:
    writeBitsForNumber(op->vSLICE);
    break;
  case SWAP:
    writeBitsForNumber(op->vSWAP.listIndex);
    writeBitsForOp(op->vSWAP.op);
    break;
  }
}

void writeBitsForNamedOp(NamedOp namedOp) {
  writeBitsForString(namedOp.name);
  writeBitsForOp(namedOp.op);
}

void writeBitsForList(OpList opList) {
  writeBitsForNumber(opList.len);
  for (u32 i = 0; i < opList.len; i++) {
    writeBitsForNamedOp(opList.ops[i]);
  }
}

void writeBitsForLists(MultiOpList multiOpList) {
  writeBitsForNumber(multiOpList.len);
  for (u32 i = 0; i < multiOpList.len; i++) {
    writeBitsForList(multiOpList.opLists[i]);
  }
}
