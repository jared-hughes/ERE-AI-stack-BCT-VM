#include "array_list.h"

ArrayList empty() {
  ArrayList list = {.length = 0, .elems = calloc(16, sizeof(Element))};
  return list;
}

void append(ArrayList list, Element elem) {
  list.elems = realloc(list.elems, list.length + 1);
  list.elems[list.length] = elem;
  list.length++;
}

u32 length(ArrayList list) { return list.length; }

Element get(ArrayList list, u32 idx) { return list.elems[idx]; }
