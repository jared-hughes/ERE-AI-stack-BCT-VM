#include "lists.h"

void appendIntList(IntList *a, int x) {
  a->len++;
  a->elems = realloc(a->elems, sizeof(u32) * a->len);
  a->elems[a->len - 1] = x;
}

IntList emptyIntList() { return (IntList){.len = 0, .elems = malloc(1)}; }

LongBound intersectBounds(LongBound a, LongBound b) {
  LongBound out = {.min = a.min < b.min ? b.min : a.min,
                   .max = a.max < b.max ? a.max : b.max};
  if (out.max < out.min) {
    epf("Impossible: max < min.");
    exit(1);
  }
  return out;
}

void appendLongBoundList(LongBoundList *a, LongBound x) {
  a->len++;
  a->bounds = realloc(a->bounds, sizeof(LongBound) * a->len);
  a->bounds[a->len - 1] = x;
}

LongBoundList emptyLongBoundList() {
  return (LongBoundList){.len = 0, .bounds = malloc(1)};
}
