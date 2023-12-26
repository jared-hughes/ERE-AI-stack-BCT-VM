#pragma once

#include <stdio.h>

#include "globals.h"
#include "op.h"

#define fpf(A...) fprintf(out, A)

void printOp(Op *op);
void printLists();
