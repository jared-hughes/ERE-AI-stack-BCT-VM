#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "op.h"
#include "print.h"

typedef enum TokenTag {
  TEOF,
  BITSTRING, // '01001'
  OPAREN,    // (
  KSWAP,     // swap
  KSLICE,    // slice
  CPAREN,    // )
  DOTDOT,    // ..
  INTEGER    // 71
} TokenTag;

typedef struct PToken {
  TokenTag tag;
  char *text;
} PToken;

typedef struct Lexer {
  char *str;
  char *pos;
  PToken curr;
  bool has_curr;
} Lexer;

Lexer emptyLexer(char *str);
PToken lexer_peek(Lexer *lexer);
PToken lexer_consume(Lexer *lexer);
PToken lexer_consumeExpectTag(Lexer *lexer, TokenTag tag);
