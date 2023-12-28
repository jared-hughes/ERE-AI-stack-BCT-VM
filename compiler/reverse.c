#include "reverse.h"

const bool DEBUG_REVERSE = false;

typedef struct WeightCtx {
  u32 weightIndex;
  int weight[512][2];
  u32 prop;
} WeightCtx;

WeightCtx initialWeightCtx = {
    .weightIndex = 0, .weight = {0}, .prop = 0xFFFFFFFF};

void updateWeights2(WeightCtx *ctx, bool y) {
  // Increment one weight of the pair.
  // Halve both if it exceeds 28 bits of 1s.
  if (++ctx->weight[ctx->weightIndex][y] > 0xFFFFFFF) {
    ctx->weight[ctx->weightIndex][0] >>= 1;
    ctx->weight[ctx->weightIndex][1] >>= 1;
  }
  // Left-shift the weight index, shifting in `y` as the LSB.
  ctx->weightIndex = ((ctx->weightIndex << 1) | y) & 0xFF;
  // Adjust prop to (roughly) the second weight of the pair, divided by the
  // sum.
  ctx->prop =
      (((u64)ctx->weight[ctx->weightIndex][1] + 1) << 32) /
      (ctx->weight[ctx->weightIndex][0] + ctx->weight[ctx->weightIndex][1] + 2);
}

/**
 * Return list `bounds` such that we need
 *   bounds[i].min <= chars[i:i+7] <= bounds[i].max
 */
LongBoundList xBoundsFromBits(IntList bits) {
  WeightCtx weightCtx = initialWeightCtx;
  LongBoundList b = emptyLongBoundList();
  appendLongBoundList(&b, longBoundFull);
  u32 j = 0;
  u64 lo = 0, hi = 0xffffffffffffffffLL;
  for (u64 i = 0; i < bits.len; i++) {
    bool y = bits.elems[i];
    u64 m = lo + ((hi - lo) >> 32) * weightCtx.prop;
    if (DEBUG_REVERSE) {
      cpf("lo=0x%016llx, hi=0x%016llx, prop=0x%08x", lo, hi, weightCtx.prop);
      cpf(" -> m=0x%016llx; y=%d\n", m, y);
    }
    if (y) {
      hi = m;
      if (DEBUG_REVERSE)
        cpf("Intersect hi %16llx: %d\n", m, j);
      // Force x <= m to be true
      b.bounds[j] =
          intersectBounds(b.bounds[j], (LongBound){.min = 0, .max = m});
    } else {
      lo = m + 1;
      if (DEBUG_REVERSE)
        cpf("Intersect lo %16llx: %d\n", m + 1, j);
      // Force x <= m to be false, i.e. x >= m+1 is true.
      b.bounds[j] = intersectBounds(
          b.bounds[j], (LongBound){.min = m + 1, .max = longBoundFull.max});
    }
    // ======== updateWeights(int y) ========
    updateWeights2(&weightCtx, y);
    // =====================================
    // While lo and hi are equal in the top 8 bits:
    while (((lo ^ hi) & 0xff00000000000000LL) == 0) {
      // Shift out those 8 bits, filling in with 0s (for lo) and 1s (for hi).
      lo = lo << 8;
      hi = hi << 8 | 0xff;
      // x = (x << 8) | c;
      appendLongBoundList(&b, longBoundFull);
      j++;
    }
  }
  return b;
}

IntList charsFromXBounds(LongBoundList bounds) {
  if (bounds.len == 0) {
    epf("Empty long bounds.");
    exit(1);
  }
  IntList chars = emptyIntList();
  for (u32 i = 0; i < 7; i++) {
    appendLongBoundList(&bounds, longBoundFull);
  }
  for (u64 i = 0; i < bounds.len; i++) {
    LongBound bound = bounds.bounds[i];
    if (DEBUG_REVERSE)
      cpf("bound {min=%16llx,max=%16llx} at %p\n", bound.min, bound.max,
          &bounds.bounds[i]);
    LongBound *nextBound = &bounds.bounds[i + 1];
    u64 topByteMax = bound.max >> 56;
    u64 topByteMin = bound.min >> 56;
    if (topByteMax - topByteMin == 0) {
      if (DEBUG_REVERSE)
        cpf("Char 0x%02llX (Shifting zero)\n", topByteMin);
      appendIntList(&chars, topByteMin);
      *nextBound =
          intersectBounds(*nextBound, (LongBound){.min = bound.min << 8,
                                                  .max = bound.max << 8});
    } else if (topByteMax - topByteMin == 1) {
      if (DEBUG_REVERSE)
        cpf("Char 0x%02llX (Shifting one. Guessing low)\n", topByteMin);
      // GUESS. TODO: branch.
      appendIntList(&chars, topByteMin);
      *nextBound =
          intersectBounds(*nextBound, (LongBound){.min = bound.min << 8,
                                                  .max = longBoundFull.max});
    } else {
      u8 ch = (bound.min << 8) == 0 ? topByteMin : topByteMin + 1;
      if (DEBUG_REVERSE)
        cpf("Char 0x%02X (No shift needed)\n", ch);
      // Easy case. topByteMax - topByteMin >= 1
      appendIntList(&chars, ch);
    }
  }
  return chars;
}

IntList charsForBits(IntList bits) {
  IntList chars = charsFromXBounds(xBoundsFromBits(bits));
  // Replace trailing 0s with EOF.
  while (chars.len > 0 && chars.elems[chars.len - 1] == 0) {
    chars.len--;
  }
  return chars;
}
