#include "lexer.h"

Lexer emptyLexer(char *str) {
  Lexer lexer;
  cpf("Reading string '%s'\n", str);
  lexer.str = str;
  lexer.pos = str;
  lexer.has_curr = false;
  return lexer;
}

char peek_char(Lexer *lexer) { return *lexer->pos; }

char consume_char(Lexer *lexer) {
  char c = *lexer->pos;
  if (c != '\0')
    lexer->pos++;
  return c;
}

// If succeeds (true), then pos will point to after the last character matching
// `exp`.
bool try_str(Lexer *lexer, char *exp) {
  u32 len = strlen(exp);
  bool good = true;
  for (u32 i = 0; i < len; i++) {
    if (lexer->pos[i] != exp[i]) {
      good = false;
      break;
    }
  }
  if (good) {
    lexer->pos += len;
  }
  return good;
}

PToken lexer_next(Lexer *lexer) {
  char c = consume_char(lexer);
  switch (c) {
  case '\0':
    lexer->pos--;
    return (PToken){.tag = TEOF, .text = ""};
  case '(':
    return (PToken){.tag = OPAREN, .text = "("};
  case ')':
    return (PToken){.tag = CPAREN, .text = ")"};
  case '.':
    if (try_str(lexer, ".")) {
      return (PToken){.tag = DOTDOT, .text = ".."};
    } else {
      epf("Expected '.' after '.', but got '%c'\n", consume_char(lexer));
      exit(1);
    }
  case 's':
    if (try_str(lexer, "wap")) {
      return (PToken){.tag = KSWAP, .text = "swap"};
    } else if (try_str(lexer, "lice")) {
      return (PToken){.tag = KSLICE, .text = "slice"};
    } else {
      epf("Expected 'swap' or 'slice' to complete 's'\n");
      exit(1);
    }
  case '0' ... '9': {
    char *val = malloc(2);
    val[0] = c;
    u32 len = 1;
    char d;
    while ('0' <= (d = peek_char(lexer)) && d <= '9') {
      len++;
      val = realloc(val, len + 1);
      val[len - 1] = consume_char(lexer);
    }
    if (c == '0' && len > 1) {
      epf("Can't start decimal constant with 0\n");
      exit(1);
    }
    val[len] = '\0';
    return (PToken){.tag = INTEGER, .text = val};
  }
  case '\'': {
    char *val = malloc(1);
    u32 len = 0;
    char d;
    while ('0' == (d = peek_char(lexer)) || '1' == d) {
      len++;
      val = realloc(val, len + 1);
      val[len - 1] = consume_char(lexer);
    }
    val[len] = '\0';
    if ((d = consume_char(lexer)) != '\'') {
      epf("String terminated with '%c' but expected \"'\"", d);
      exit(1);
    }
    return (PToken){.tag = BITSTRING, .text = val};
  }
  case ' ':
  case '\t':
    return lexer_next(lexer);
  default:
    epf("Invalid char '%c' (0x%02X)\n", c, c);
    exit(1);
  }
}

PToken lexer_peek(Lexer *lexer) {
  if (!lexer->has_curr) {
    lexer->curr = lexer_next(lexer);
    lexer->has_curr = true;
  }
  return lexer->curr;
}

PToken lexer_consume(Lexer *lexer) {
  if (!lexer->has_curr)
    return lexer_next(lexer);
  lexer->has_curr = false;
  return lexer->curr;
}

PToken lexer_consumeExpectTag(Lexer *lexer, TokenTag tag) {
  PToken next = lexer_consume(lexer);
  if (next.tag != tag) {
    epf("Expected tag %d but got '%s'\n", tag, next.text);
    exit(1);
  }
  return next;
}
