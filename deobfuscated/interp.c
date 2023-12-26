#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "op.c"

FILE *in;

typedef unsigned long long u64;
typedef unsigned int u32;

u64 l, h = 0xffffffffffffffffLL, x;

u32 cxt, _pr = 0xFFFFFFFF;

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

bool rb() {
  u64 m = l + ((h - l) >> 32) * _pr;
  bool y = x <= m;
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

// RLE?
u32 getLen() {
  u32 l2 = 0;
  while (rb())
    l2++;
  u32 v = 0;
  while (l2--)
    v = (v << 1) | rb();
  return v;
}

// stacks[i] is a stack of ops.
OpWithStr **stacks;
// midLens[i] is the length of stacks[i]
u32 *midLens;
// topLen is the length of the arrays stacks and midLens.
u32 topLen;

Op *pe() {
  char t = rb() * 2 + rb();
  switch (t) {
  case BIN:
    return makeBIN(pe(), pe());
  case STR:
    u32 len = getLen();
    char *v = calloc(len + 1, 1);
    for (int i = 0; i < len; i++) {
      v[i] = rb() ? '1' : '0';
    };
    return makeSTR(v);
  case INT:
    return makeINT(getLen());
  case CALL:
    u32 x = getLen();
    return makeCALL(x, pe());
  }
}

void top() {
  topLen = getLen();
  stacks = calloc(topLen, sizeof(OpWithStr *));
  midLens = calloc(topLen, sizeof(OpWithStr));
  for (int i = 0; i < topLen; i++) {
    midLens[i] = getLen();
    stacks[i] = calloc(midLens[i], sizeof(OpWithStr));
    for (int j = 0; j < midLens[i]; j++) {
      OpWithStr *stack = &stacks[i][j];
      u32 botLen = getLen();
      stack->str = calloc(botLen + 1, 1);
      for (int x = 0; x < botLen; x++) {
        stack->str[x] = rb() ? '1' : '0';
      }
      stack->op = pe();
    };
  }
}

/** Is w a prefix of z?
 * E.g. "abc" is a prefix of "abc" and "abcdef" but not "ab". */
bool isPrefix(char *w, char *z) {
  while (*w)
    if (*w != *z) {
      return false;
    } else {
      w++, z++;
    }
  return true;
}

char *eval(int stackIndex, char *in);

char *de(Op *e, char *in) {
  switch (e->tag) {
  case BIN:
    char *l = de(e->t0.r, in), *r = de(e->t0.l, in),
         *o = malloc(strlen(l) + strlen(r) + 1);
    strcpy(o, l);
    strcat(o, r);
    free(l);
    free(r);
    return o;
  case STR:
    return strdup(e->t1);
  case INT:
    return strdup(!e->t2 ? in : e->t2 >= strlen(in) ? "" : in + e->t2);
  case CALL:
    return eval(e->t3.stackIndex, de(e->t3.args, in));
  }
}

char *eval(int stackIndex, char *in) {
  OpWithStr *stack = stacks[stackIndex];
re:
  for (int j = 0; j < strlen(in); j++) {
    for (int i = 0; i < midLens[stackIndex]; i++) {
      if (isPrefix(stack[i].str, in + j)) {
        char *o = de(stack[i].op, in + j + strlen(stack[i].str));
        char *ni =
            malloc(strlen(o) + strlen(in) - strlen(stack[i].str) + 1 + j);
        strncpy(ni, in, j);
        ni[j] = 0;
        strcat(ni, o);
        strcat(ni, in + j + strlen(stack[i].str));
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
  // Build a u64 x from the first 8 bytes.
  for (int i = 0; i < 8; i++) {
    int c = getc(in);
    if (c == EOF)
      c = 0;
    x = x << 8 | c;
  }
  top();
  char *ni = malloc(strlen(av[2]) + 5);
  strcpy(ni, "000");
  strcat(ni, av[2]);
  char *r = eval(0, ni);
  printf("%s\n", r);
}
