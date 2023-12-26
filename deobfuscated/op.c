typedef enum OpTag { BIN = 0, STR = 1, INT = 2, CALL = 3 } OpTag;

typedef struct Op {
  OpTag tag;
  union {
    struct {
      struct Op *l, *r;
    } vBIN;
    char *vSTR;
    int vINT;
    struct {
      int stackIndex;
      struct Op *arg;
    } vCALL;
  };
} Op;

Op *_makeBlank(OpTag tag) {
  Op *e = malloc(sizeof(Op));
  e->tag = tag;
  return e;
}

// 0: binop
Op *makeBIN(Op *l, Op *r) {
  Op *e = _makeBlank(BIN);
  e->vBIN.l = l;
  e->vBIN.r = r;
  return e;
}

// 1: string constant.
Op *makeSTR(char *v) {
  Op *e = _makeBlank(STR);
  e->vSTR = v;
  return e;
}

// 2: integer constant.
Op *makeINT(int r) {
  Op *e = _makeBlank(INT);
  e->vINT = r;
  return e;
}

// 3: op index plus arg.
Op *makeCALL(int stackIndex, Op *arg) {
  Op *e = _makeBlank(CALL);
  e->vCALL.stackIndex = stackIndex;
  e->vCALL.arg = arg;
  return e;
}

typedef struct OpWithStr {
  char *vSTR;
  Op *op;
} OpWithStr;
