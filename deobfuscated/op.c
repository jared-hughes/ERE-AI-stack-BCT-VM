

typedef struct Op {
  int tag;
  union {
    struct {
      struct Op *l, *r;
    } t0;
    char *t1;
    int t2;
    struct {
      int f;
      struct Op *a;
    } t3;
  };
} Op;

Op *_makeBlank(int tag) {
  Op *e = malloc(sizeof(Op));
  e->tag = tag;
  return e;
}

Op *make0(Op *l, Op *r) {
  Op *e = _makeBlank(0);
  e->t0.l = l;
  e->t0.r = r;
  return e;
}

Op *make1(char *v) {
  Op *e = _makeBlank(1);
  e->t1 = v;
  return e;
}

Op *make2(int r) {
  Op *e = _makeBlank(2);
  e->t2 = r;
  return e;
}

Op *make3(int f, Op *a) {
  Op *e = _makeBlank(3);
  e->t3.f = f;
  e->t3.a = a;
  return e;
}

typedef struct Ops {
  char *s;
  Op *node;
} Ops;
