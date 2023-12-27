#pragma once

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "op.h"

#define fpf(A...) fprintf(out, A)

void printOp(Op *op);
void printLists();
void printProp(u32 prop);
char *nameTag(OpTag tag);
