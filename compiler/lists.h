#pragma once

#include "op.h"
#include "print.h"

typedef struct IntList {
  u32 len;
  int *elems;
} IntList;

// Inclusive bounds.
typedef struct LongBound {
  u64 min;
  u64 max;
} LongBound;

static const LongBound longBoundFull = {.min = 0, .max = 0xFFFFFFFFFFFFFFFF};

typedef struct LongBoundList {
  LongBound *bounds;
  u32 len;
} LongBoundList;

LongBound intersectBounds(LongBound a, LongBound b);

void appendIntList(IntList *a, int x);
IntList emptyIntList();

void appendLongBoundList(LongBoundList *a, LongBound x);
LongBoundList emptyLongBoundList();
