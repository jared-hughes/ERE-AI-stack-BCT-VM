#include "lists.h"

void appendIntList(IntList *a, int x) {
  a->len++;
  a->elems = realloc(a->elems, sizeof(u32) * a->len);
  a->elems[a->len - 1] = x;
}

IntList emptyIntList() { return (IntList){.len = 0, .elems = malloc(1)}; }

void appendBoundList(BoundList *a, Bound x) {
  a->len++;
  a->bounds = realloc(a->bounds, sizeof(Bound) * a->len);
  a->bounds[a->len - 1] = x;
}

BoundList emptyBoundList() {
  return (BoundList){.len = 0, .bounds = malloc(1)};
}
