#include "parse.h"

bool is_whitespace(char c) { return c == '\n' || c == ' ' || c == '\t'; }

void my_getline(char **line) {
  u32 len = 0;
  while (!feof(in)) {
    char c = fgetc(in);
    // Skip leading newlines and whitsepace.
    if (len == 0 && is_whitespace(c))
      continue;
    len++;
    *line = realloc(*line, len);
    if (c == '\n') {
      (*line)[len - 1] = '\0';
      return;
    }
    (*line)[len - 1] = c;
  }
}

bool assertBitstring(char *str) {
  for (; *str; ++str) {
    charToBit(*str);
  }
}

int parseInt(Lexer *lexer) {
  PToken tok = lexer_consumeExpectTag(lexer, INTEGER);
  return atoi(tok.text);
}

Op *parseMain(Lexer *lexer);

Op *parseInitial(Lexer *lexer) {
  PToken fst = lexer_consume(lexer);
  if (fst.tag == EOF) {
    epf("Early EOF.\n");
    exit(1);
  }
  switch (fst.tag) {
  case BITSTRING:
    char *str = fst.text;
    if (*str == '\'') {
      str++;
      str[strlen(str) - 1] = '\0';
    }
    return makeSTR(str);
  case OPAREN:
    PToken snd = lexer_consume(lexer);
    switch (snd.tag) {
    case KSWAP:
      int listIndex = parseInt(lexer);
      Op *op = parseMain(lexer);
      lexer_consumeExpectTag(lexer, CPAREN);
      return makeSWAP(listIndex, op);
    case KSLICE:
      int slice = parseInt(lexer);
      lexer_consumeExpectTag(lexer, CPAREN);
      return makeSLICE(slice);
    default:
      epf("Invalid token after '(': '%s'\n", snd.text);
      exit(1);
    }
    break;
  default:
    epf("Invalid token: '%s'\n", fst.text);
    exit(1);
  }
}

Op *parseDotDot(Lexer *lexer, Op *leftNode) {
  Op *rightNode = parseMain(lexer);
  return makeCAT(leftNode, rightNode);
}

Op *parseMain(Lexer *lexer) {
  Op *leftNode = parseInitial(lexer);
  while (true) {
    PToken tok = lexer_peek(lexer);
    // No need for binding power since there's only one infix op.
    if (tok.tag != DOTDOT)
      break;
    lexer_consume(lexer);
    leftNode = parseDotDot(lexer, leftNode);
  }
  return leftNode;
}

Op *parseOp(char *str) {
  Lexer lexer = emptyLexer(str);
  Op *ret = parseMain(&lexer);
  return ret;
}

NamedOp parseNamedOp(char *line) {
  char *colon = strchr(line, ':');
  if (colon == NULL) {
    epf("Parse error: Line missing ':'. Line:\n");
    epf("%s\n", line);
    exit(1);
  }
  *colon = '\0';
  assertBitstring(line);
  return (NamedOp){.name = strdup(line), .op = parseOp(colon + 1)};
}

MultiOpList parseMultiOpList() {
  MultiOpList multiOpList;
  multiOpList.len = 0;
  multiOpList.opLists = calloc(1, sizeof(OpList));
  char *line = calloc(10, 1);
  while (!feof(in)) {
    my_getline(&line);
    if (feof(in))
      break;
    if (line[0] == '=') {
      appendMultiOpList(&multiOpList, emptyOpList());
    } else if (multiOpList.len == 0) {
      epf("Parse error: must start file with an '='. Line:\n");
      epf("%s\n", line);
      exit(1);
    } else {
      OpList *opList = &multiOpList.opLists[multiOpList.len - 1];
      appendOpList(opList, parseNamedOp(line));
    }
  }
  return multiOpList;
}
