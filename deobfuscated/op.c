#include "op.h"

Op *_makeBlank(OpTag tag) {
  Op *e = malloc(sizeof(Op));
  e->tag = tag;
  return e;
}

// 0: execute children, then concatenate
Op *makeCAT(Op *l, Op *r) {
  Op *e = _makeBlank(CAT);
  e->vCAT.l = l;
  e->vCAT.r = r;
  return e;
}

// 1: string constant.
Op *makeSTR(char *v) {
  Op *e = _makeBlank(STR);
  e->vSTR = v;
  return e;
}

// 2: integer constant.
Op *makeSLICE(int r) {
  Op *e = _makeBlank(SLICE);
  e->vSLICE = r;
  return e;
}

// 3: execute child op with a different (fixed) list index.
Op *makeSWAP(int listIndex, Op *op) {
  Op *e = _makeBlank(SWAP);
  e->vSWAP.listIndex = listIndex;
  e->vSWAP.op = op;
  return e;
}
