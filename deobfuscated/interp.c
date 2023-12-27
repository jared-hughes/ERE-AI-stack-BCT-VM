#include "interp.h"

const bool DEBUG = true;
const bool DEBUG_INPUT = true && DEBUG;
const bool DEBUG_INPUT_LOW = false && DEBUG;
const bool DEBUG_INPUT_MID = true && DEBUG;
const bool DEBUG_OUTPUT = false && DEBUG;

u64 lo = 0, hi = 0xffffffffffffffffLL, x = 0;

u32 prop = 0xFFFFFFFF;

/** weight[ctx] is a pair of ints. */
u32 weightIndex;
int weight[512][2];

// opLists[i] is a list of named ops.
NamedOp **opLists;
// midLens[i] is the length of opLists[i]
u32 *midLens;
// topLen is the length of the arrays opLists and midLens.
u32 topLen;

void updateWeights(int y) {
  // Increment one weight of the pair.
  // Halve both if it exceeds 28 bits of 1s.
  if (++weight[weightIndex][y] > 0xFFFFFFF) {
    weight[weightIndex][0] >>= 1;
    weight[weightIndex][1] >>= 1;
  }
  // Left-shift the weight index, shifting in `y` as the LSB.
  weightIndex = (weightIndex << 1) | y;
  weightIndex &= 0xff;
  // Adjust prop to (roughly) the second weight of the pair, divided by the sum.
  prop = (((u64)(weight[weightIndex][1] + 1) << 32) /
          (weight[weightIndex][0] + weight[weightIndex][1] + 2));
}

/**
 * Generate a number `m` between `lo` and `hi` by
 * lerping `prop/(1<<32)` of the way from `lo` to `hi`.
 *
 * Treat it as a binary search for 0 in a sorted array `arr`,
 * where `arr[lo] < 0 = arr[x] = 0 < arr[hi]`.
 * But the algorithm doesn't know the exact value of `x`.
 * It just knows `lo < x < hi` and the comparison `x <= m`.
 * Update `lo`, `hi` correctly, and return 1 if `x <= m`, else 0.
 */
bool randomBit() {
  u64 m = lo + ((hi - lo) >> 32) * prop;
  bool y = x <= m;
  if (DEBUG_INPUT_LOW) {
    fpf("lo=0x%016llx, hi=0x%016llx, prop=0x%08x=", lo, hi, prop);
    printProp(prop);
    fpf(" -> m=0x%016llx; x=0x%016llx; y=%d\n", m, x, y);
  } else if (DEBUG_INPUT_MID) {
    fpf("Read bit %d\n", y);
  }
  if (y)
    hi = m;
  else
    lo = m + 1;
  updateWeights(y);
  // While lo and hi are equal in the top 8 bits:
  while (((lo ^ hi) & 0xff00000000000000LL) == 0) {
    // Shift out those 8 bits, filling in with 0s (for lo) and 1s (for hi).
    u64 old_lo = lo;
    u64 old_hi = hi;
    lo = lo << 8;
    hi = hi << 8 | 0xff;
    // Shift out the top 8 bits of x, shifting in with getc(in).
    int c = getc(in);
    if (c == EOF)
      c = 0;
    if (DEBUG_INPUT_LOW) {
      fpf("Shifting in 0x%02x because lo=0x%016llx, hi=0x%016llx.\n", c, old_lo,
          old_hi);
    }
    x = x << 8 | c;
  }
  return y;
}

/**
 * Pick l2 based on a geometric distribution p=1/2, i.e.
 * P(l2=0)=1/2, P(l2=1)=1/4, P(l2=2)=1/8, ...
 * Then pick v uniformly between 0 and (1<<l2)-1 inclusive,
 * by randomly toggling individual bits.
 * This gives the following distribution:
 * (Calculated by https://www.desmos.com/calculator/kwkjup4tt4)
 * 0   :   2/3
 * 1   :   1/6
 * 2-3 :   1/24 each
 * 4-7 :   1/96 each
 * 8-15:   1/384 each
 */
u32 randomUnbounded() {
  if (DEBUG_INPUT_MID) {
    fpf("Making a random unbounded unsigned int.\n");
  }
  u32 l2 = 0;
  while (randomBit())
    l2++;
  u32 v = 0;
  while (l2--)
    v = (v << 1) | randomBit();
  if (DEBUG_INPUT_MID) {
    fpf("Got %u.\n", v);
  }
  return v;
}

Op *makeRandom() {
  if (DEBUG_INPUT_MID) {
    fpf("Making a random op.\n");
  }
  char t = randomBit() * 2 + randomBit();
  if (DEBUG_INPUT_MID) {
    fpf("Opcode:%5s=%d.\n", nameTag(t), t);
  }
  switch (t) {
  case CAT:
    return makeCAT(makeRandom(), makeRandom());
  case STR:
    u32 len = randomUnbounded();
    char *v = calloc(len + 1, 1);
    for (int i = 0; i < len; i++) {
      v[i] = randomBit() ? '1' : '0';
    };
    return makeSTR(v);
  case SLICE:
    return makeSLICE(randomUnbounded());
  case SWAP:
    u32 x = randomUnbounded();
    return makeSWAP(x, makeRandom());
  }
}

void initLists() {
  topLen = randomUnbounded();
  opLists = calloc(topLen, sizeof(NamedOp *));
  midLens = calloc(topLen, sizeof(NamedOp));
  for (int i = 0; i < topLen; i++) {
    midLens[i] = randomUnbounded();
    opLists[i] = calloc(midLens[i], sizeof(NamedOp));
    for (int j = 0; j < midLens[i]; j++) {
      NamedOp *opList = &opLists[i][j];
      u32 botLen = randomUnbounded();
      opList->name = calloc(botLen + 1, 1);
      for (int x = 0; x < botLen; x++) {
        opList->name[x] = randomBit() ? '1' : '0';
      }
      if (DEBUG_INPUT_MID) {
        fpf("Name: %s\n", opList->name);
      }
      opList->op = makeRandom();
      if (DEBUG_INPUT_MID) {
        fpf("Created %s: ", opList->name);
        printOp(opList->op);
        fpf("\n");
      }
    };
  }
}

/** Is string w a prefix of string z?
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

char *eval(int listIndex, char *in);

char *toString(Op *e, char *in) {
  switch (e->tag) {
  case CAT:
    // Get two results and concatenate them.
    char *l = toString(e->vCAT.r, in), *r = toString(e->vCAT.l, in),
         *o = malloc(strlen(l) + strlen(r) + 1);
    strcpy(o, l);
    strcat(o, r);
    free(l);
    free(r);
    return o;
  case STR:
    // Return this constant string.
    return strdup(e->vSTR);
  case SLICE:
    // Return `in[e->vSLICE:]` (Python string slice notation).
    // First line is unnecessary? Dupliates 'else' clause.
    return strdup(!e->vSLICE                ? in
                  : e->vSLICE >= strlen(in) ? ""
                                            : in + e->vSLICE);
  case SWAP:
    // Recursive eval call.
    return eval(e->vSWAP.listIndex, toString(e->vSWAP.op, in));
  }
}

// Repeat until fixed point:
//  1. Scan from left-to-right in the string until finding a matching op.
//  2. Go through the whole op list in order.
//    Find the first op whose name is a prefix of that part in the string.
//  3. Replace the the op name with the evaluation result
//    (toString the op, with `in` being the remainder of the string.)
//  4. Repeat, back at the beginning of the string.
char *eval(int listIndex, char *in) {
  NamedOp *opList = opLists[listIndex];
re:
  if (DEBUG_OUTPUT) {
    fpf("%s\n", in);
  }
  for (int j = 0; j < strlen(in); j++) {
    for (int i = 0; i < midLens[listIndex]; i++) {
      NamedOp namedOp = opList[i];
      char *name = namedOp.name;
      Op *op = namedOp.op;
      if (isPrefix(name, in + j)) {
        // if (DEBUG_OUTPUT) {
        //   fpf("Applying %s: ", name);
        //   printOp(op);
        //   fpf("\n");
        // }
        char *res = toString(op, in + j + strlen(name));
        char *ni = malloc(strlen(res) + strlen(in) - strlen(name) + 1 + j);
        strncpy(ni, in, j);
        ni[j] = 0;
        strcat(ni, res);
        strcat(ni, in + j + strlen(name));
        free(in);
        free(res);
        in = ni;
        goto re;
      };
    };
  }

  return in;
}

int interp_main(int argc, char *argv[]) {
  initFiles(argv[1]);
  // Build a u64 x from the first 8 bytes.
  for (int i = 0; i < 8; i++) {
    int c = getc(in);
    if (c == EOF)
      c = 0;
    x = x << 8 | c;
  }
  // Initialize opLists using the rest of the program file.
  initLists();
  if (DEBUG_INPUT) {
    fpf("\n");
    printLists();
    writeBitsForLists(topLen, midLens, opLists);
  }
  if (DEBUG_OUTPUT)
    fpf("=== OUTPUT ===\n\n");
  // Prepare the initial string being the second argument, with 000 prepended.
  char *ni = malloc(strlen(argv[2]) + 5);
  strcpy(ni, "000");
  strcat(ni, argv[2]);
  // Evaluate, starting from the 0th opList.
  char *r = eval(0, ni);
  printf("%s\n", r);
}
