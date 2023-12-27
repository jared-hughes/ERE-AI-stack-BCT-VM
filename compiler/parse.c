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

/** Replace trailing spaces with null terminator, and return pointer to first
 * non-space.*/
char *trimSpaces(char *str) {
  for (; *str && is_whitespace(*str);) {
    str++;
  }
  char *end = str + strlen(str) - 1;
  for (; *end && is_whitespace(*end); end--) {
    *end = '\0';
  }
  return str;
}

bool assertBitstring(char *str) {
  for (; *str; ++str) {
    charToBit(*str);
  }
}

Op *parseOp(char *str) {
  str = trimSpaces(str);
  if (str[0] != '\'') {
    epf("Parse error: expected string! Str:\n");
    epf("%s\n", str);
    exit(1);
  }
  str++;
  int len = strlen(str);
  // For now, only parse strings.
  if (str[len - 1] != '\'') {
    epf("Parse error: expected string. Str:\n");
    epf("%s\n", str);
    exit(1);
  }
  str[len - 1] = '\0';
  assertBitstring(str);
  return makeSTR(strdup(str));
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
