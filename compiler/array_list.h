#pragma once

#include "op.h"

typedef u32 Element;
typedef struct ArrayList {
  u32 length;
  Element *elems;
} ArrayList;

ArrayList empty();
void append(ArrayList list, Element elem);
u32 length(ArrayList list);
Element get(ArrayList list, u32 idx);
