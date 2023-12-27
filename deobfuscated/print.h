#pragma once

#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "op.h"

extern FILE *in;
extern FILE *out;
extern FILE *compilerLogOut;

#define epf(A...) fprintf(stderr, A)
#define cpf(A...) fprintf(compilerLogOut, A)
#define fpf(A...) fprintf(out, A)

void initFiles(char *inFileName);

void cprintOp(Op *op);
void printOp(Op *op);
void printLists(MultiOpList multiOpList);
void printProp(u32 prop);
char *nameTag(OpTag tag);
