#include "../include/mystring.h"

char* Strchr(String* str, char ch) {
  char* move = str->str;
  while (move < str->str + str->len) {
    if (*move == ch) {
      return move;
    }

    move++;
  }

  return nullptr;
}

int Strcmp(String* strA, String* strB) {
  return strncmp(strA->str, strB->str, (strA->len < strB->len) ? strA->len : strB->len);
}

int Fputs(String* str, FILE* file) {
  char* move = str->str;

  while (move < str->str + str->len) {
    if (fputc(*move, file) == EOF) {
      return EOF;
    }

    move++;
  }

  return (int)str->len;
}

