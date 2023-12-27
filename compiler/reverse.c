#include "reverse.h"

/** Returns list of u32 values corresponding to the successive expected `prop`
 * values. */
// u32 *computeProps(IntList bits) {
//   u32 weightIndex;
//   int weight[512][2];
//   u32 prop = 0xFFFFFFFF;

//   u32 *props = calloc(bits.len, sizeof(u32));

//   for (u32 i = 0; i < bits.len; i++) {
//     props[i] = prop;
//     bool y = bits.elems[i];

//     // ======== updateWeights(int y) ========
//     // Increment one weight of the pair.
//     // Halve both if it exceeds 28 bits of 1s.
//     if (++weight[weightIndex][y] > 0xFFFFFFF) {
//       weight[weightIndex][0] >>= 1;
//       weight[weightIndex][1] >>= 1;
//     }
//     // Left-shift the weight index, shifting in `y` as the LSB.
//     weightIndex = (weightIndex << 1) | y;
//     weightIndex &= 0xff;
//     // Adjust prop to (roughly) the second weight of the pair, divided by the
//     // sum.
//     prop = (((u64)(weight[weightIndex][1] + 1) << 32) /
//             (weight[weightIndex][0] + weight[weightIndex][1] + 2));
//   }
//   return props;
// }

void impossible(Bound *xPtr, u64 m) {
  for (u32 i = 0; i < 8; i++) {
    epf("(%02x..%02x)", xPtr[i].min, xPtr[i].max);
  }
  epf("\n");
  for (u32 i = 8; i--;) {
    epf("(  %02llx  )", m >> (i * 8) & 0xFF);
  }
  epf("\n");
}

void coerceLeq(Bound *xPtr, u64 m) {
  // Decrease the first max to guarantee xPtr[0:7] <= m.
  // TODO: Consider not subtracting 1
  // THIS IS NEEDED. Might even need to set up several alternatives.
  u8 max = (m >> 56) - 1;
  if (xPtr[0].min <= max) {
    xPtr[0].max = max;
  } else {
    epf("Impossible situation in coerceLeq.\n");
    impossible(xPtr, m);
    exit(1);
  }
}

void coerceGt(Bound *xPtr, u64 m) {
  // Increase the first min to guarantee xPtr[0:7] > m.
  // TODO: Consider not adding 1.
  // THIS IS NEEDED. Might even need to set up several alternatives.
  u8 min = (m >> 56) + 1;
  if (xPtr[0].max >= min) {
    xPtr[0].min = min;
  } else {
    epf("Impossible situation in coerceGt.\n");
    impossible(xPtr, m);
    exit(1);
  }
}

/** Returns list of char values (inside u32s) to create such bits. */
IntList charsForBits(IntList bits) {
  u32 weightIndex = 0;
  int weight[512][2] = {0};
  u32 prop = 0xFFFFFFFF;
  BoundList bounds = emptyBoundList();
  for (u32 i = 0; i < 8; i++) {
    appendBoundList(&bounds, (Bound){.min = 0x00, .max = 0xFF});
  }
  Bound *xPtr = bounds.bounds;
  u64 lo = 0, hi = 0xffffffffffffffffLL;
  for (u64 i = 0; i < bits.len; i++) {
    bool y = bits.elems[i];
    u64 m = lo + ((hi - lo) >> 32) * prop;
    cpf("lo=0x%016llx, hi=0x%016llx, prop=0x%08x", lo, hi, prop);
    cpf(" -> m=0x%016llx; y=%d\n", m, y);
    if (y) {
      hi = m;
      coerceLeq(xPtr, m);
    } else {
      lo = m + 1;
      coerceGt(xPtr, m);
    }
    // ======== updateWeights(int y) ========
    // Increment one weight of the pair.
    // Halve both if it exceeds 28 bits of 1s.
    if (++weight[weightIndex][y] > 0xFFFFFFF) {
      weight[weightIndex][0] >>= 1;
      weight[weightIndex][1] >>= 1;
    }
    // Left-shift the weight index, shifting in `y` as the LSB.
    weightIndex = (weightIndex << 1) | y;
    weightIndex &= 0xff;
    // Adjust prop to (roughly) the second weight of the pair, divided by the
    // sum.
    prop = (((u64)(weight[weightIndex][1] + 1) << 32) /
            (weight[weightIndex][0] + weight[weightIndex][1] + 2));
    // =====================================
    // While lo and hi are equal in the top 8 bits:
    while (((lo ^ hi) & 0xff00000000000000LL) == 0) {
      // Shift out those 8 bits, filling in with 0s (for lo) and 1s (for hi).
      lo = lo << 8;
      hi = hi << 8 | 0xff;
      // x = (x << 8) | c;
      appendBoundList(&bounds, (Bound){.min = 0x00, .max = 0xFF});
      xPtr++;
    }
  }
  IntList chars = emptyIntList();
  for (u64 i = 0; i < bounds.len; i++) {
    appendIntList(&chars, bounds.bounds[i].min);
  }
  // Replace trailing 0s with EOF.
  while (chars.len > 0 && chars.elems[chars.len - 1] == 0) {
    chars.len--;
  }
  return chars;
}
