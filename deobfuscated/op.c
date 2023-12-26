typedef enum OpTag { BIN = 0, STR = 1, INT = 2, CALL = 3 } OpTag;

typedef struct Op {
  OpTag tag;
  union {
    struct {
      struct Op *l, *r;
    } t0;
    char *t1;
    int t2;
    struct {
      int stackIndex;
      struct Op *args;
    } t3;
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
  e->t0.l = l;
  e->t0.r = r;
  return e;
}

// 1: string constant.
Op *makeSTR(char *v) {
  Op *e = _makeBlank(STR);
  e->t1 = v;
  return e;
}

// 2: integer constant.
Op *makeINT(int r) {
  Op *e = _makeBlank(INT);
  e->t2 = r;
  return e;
}

// 3: op index plus args.
Op *makeCALL(int stackIndex, Op *a) {
  Op *e = _makeBlank(CALL);
  e->t3.stackIndex = stackIndex;
  e->t3.args = a;
  return e;
}

typedef struct OpWithStr {
  char *str;
  Op *op;
} OpWithStr;
