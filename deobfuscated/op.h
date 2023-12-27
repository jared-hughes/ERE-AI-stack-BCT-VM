#pragma once

#include <stdlib.h>

typedef unsigned long long u64;
typedef unsigned int u32;

typedef enum OpTag { CAT = 0, STR = 1, SLICE = 2, SWAP = 3 } OpTag;

typedef struct Op {
  OpTag tag;
  union {
    struct {
      struct Op *l, *r;
    } vCAT;
    char *vSTR;
    int vSLICE;
    struct {
      int listIndex;
      struct Op *op;
    } vSWAP;
  };
} Op;

typedef struct NamedOp {
  char *name;
  Op *op;
} NamedOp;

typedef struct OpList {
  u32 len;
  NamedOp *ops;
} OpList;

typedef struct MultiOpList {
  u32 len;
  OpList *opLists;
} MultiOpList;

Op *makeCAT(Op *l, Op *r);

Op *makeSTR(char *v);

Op *makeSLICE(int r);

Op *makeSWAP(int listIndex, Op *op);
