#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "op.c"

FILE *in;

unsigned long long l, h = 0xffffffffffffffffLL, x;

unsigned int cxt, _pr = 0xFFFFFFFF;

int ct[512][2];

void up(int y) {
  if (++ct[cxt][y] > 0xFFFFFFF) {
    ct[cxt][0] >>= 1, ct[cxt][1] >>= 1;
  }
  cxt = (cxt << 1) | y;
  cxt &= 0xff;
  _pr = (((unsigned long long)(ct[cxt][1] + 1) << 32) /
         (ct[cxt][0] + ct[cxt][1] + 2));
}

int rb() {
  unsigned long long m = l + ((h - l) >> 32) * _pr;
  int y = x <= m;
  if (y)
    h = m;
  else
    l = m + 1;
  up(y);
  while (((l ^ h) & 0xff00000000000000LL) == 0) {
    l = l << 8;
    h = h << 8 | 0xff;
    int c = getc(in);
    if (c == EOF)
      c = 0;
    x = x << 8 | c;
  }
  return y;
}

unsigned int eg() {
  unsigned int l2 = 0;
  while (rb())
    l2++;
  unsigned int v = 0;
  while (l2--)
    v = (v << 1) | rb();
  return v;
}

Ops **fs;

unsigned int *fns, nfs;

Op *pe() {
  char t = rb() * 2 + rb();
  switch (t) {
  case 0:
    return make0(pe(), pe());
  case 1:
    unsigned int len = eg();
    char *v = calloc(len + 1, 1);
    for (int i = 0; i < len; i++) {
      v[i] = rb() ? 49 : 48;
    };
    return make1(v);
  case 2:
    return make2(eg());
  case 3:
    unsigned int x = eg();
    return make3(x, pe());
  }
}

void pf() {
  fs = calloc(nfs = eg(), sizeof(Ops *));
  fns = calloc(nfs, sizeof(Ops));
  for (int i = 0; i < nfs; i++) {
    fs[i] = calloc(fns[i] = eg(), sizeof(Ops));
    for (int j = 0; j < fns[i]; j++) {
      Ops *op = &fs[i][j];
      unsigned int len;
      op->s = calloc((len = eg()) + 1, 1);
      for (int x = 0; x < len; x++) {
        op->s[x] = rb() ? 49 : 48;
      }
      op->node = pe();
    };
  }
}

int pre(char *w, char *z) {
  while (*w)
    if (*w != *z) {
      return 0;
    } else {
      w++, z++;
    }
  return 1;
}

char *eval(int f, char *in);

char *de(Op *e, char *in) {
  switch (e->tag) {
  case 2:
    return strdup(!e->t2 ? in : e->t2 >= strlen(in) ? "" : in + e->t2);
  case 1:
    return strdup(e->t1);
  case 0:
    char *l = de(e->t0.r, in), *r = de(e->t0.l, in),
         *o = malloc(strlen(l) + strlen(r) + 1);
    strcpy(o, l);
    strcat(o, r);
    free(l);
    free(r);
    return o;
  case 3:
    return eval(e->t3.f, de(e->t3.a, in));
  }
}

char *eval(int f, char *in) {
  Ops *ops = fs[f];
re:
  for (int j = 0; j < strlen(in); j++) {
    for (int i = 0; i < fns[f]; i++) {
      if (pre(ops[i].s, in + j)) {
        char *o = de(ops[i].node, in + j + strlen(ops[i].s));
        char *ni = malloc(strlen(o) + strlen(in) - strlen(ops[i].s) + 1 + j);
        strncpy(ni, in, j);
        ni[j] = 0;
        strcat(ni, o);
        strcat(ni, in + j + strlen(ops[i].s));
        free(in);
        free(o);
        in = ni;
        goto re;
      };
    };
  }

  return in;
}

int main(int ac, char *av[]) {
  in = fopen(av[1], "rb");
  for (int i = 0; i < 8; i++) {
    int c = getc(in);
    if (c == EOF)
      c = 0;
    x = x << 8 | c;
  }
  pf();
  char *ni = malloc(strlen(av[2]) + 5);
  strcpy(ni, "000");
  strcat(ni, av[2]);
  char *r = eval(0, ni);
  printf("%s\n", r);
}
