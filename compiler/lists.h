#pragma once

#include "op.h"

typedef struct IntList {
  u32 len;
  int *elems;
} IntList;

// Inclusive bounds.
typedef struct Bound {
  u8 min;
  u8 max;
} Bound;

typedef struct BoundList {
  Bound *bounds;
  u32 len;
} BoundList;

void appendIntList(IntList *a, int x);
IntList emptyIntList();

void appendBoundList(BoundList *a, Bound x);
BoundList emptyBoundList();
