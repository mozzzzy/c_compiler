#include <ctype.h>    // isspace
#include <stdio.h>    // fprintf
#include <stdlib.h>   // malloc
#include <string.h>   // strncmp

#include <linkedList.h>     // LinkedList

#include "error_handler.h"  // error_at
#include "tokenizer.h"


// tokenize user input and put each token to the array
void tokenize (char *user_input, LinkedList *linked_list) {
  fprintf(stderr, "tokenize start\n");

  char *p = user_input;
  fprintf(stderr, "user_input is following codes\n");
  fprintf(stderr, "8< - - 8< - - 8< - - 8<\n");
  fprintf(stderr, "%s", p);
  fprintf(stderr, "\n8< - - 8< - - 8< - - 8<\n");

  // index of the array
  while (*p) {
    // skip a space
    if (isspace(*p)) {
      fprintf(stderr, "get a space\n");
      ++p;
      continue;
    }

    Token *token = (Token *)malloc(sizeof(Token));
    // tokenize ==
    if (strncmp(p, "==", 2) == 0) {
      fprintf(stderr, "get a ==\n");
      token->ty = TK_EQ;
      addData(linked_list, token);
      p += 2;
      continue;
    }

    // tokenize !=
    if (strncmp(p, "!=", 2) == 0) {
      fprintf(stderr, "get a !=\n");
      token->ty = TK_NE;
      addData(linked_list, token);
      p += 2;
      continue;
    }

    // tokenize <=
    if (strncmp(p, "<=", 2) == 0) {
      fprintf(stderr, "get a <=\n");
      token->ty = TK_LE;
      addData(linked_list, token);
      p += 2;
      continue;
    }

    // tokenize <=
    if (strncmp(p, ">=", 2) == 0) {
      fprintf(stderr, "get a >=\n");
      token->ty = TK_GE;
      addData(linked_list, token);
      p += 2;
      continue;
    }

    // tokenize +, -, *, /, (, ), <, >
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
        || *p == '(' || *p == ')' || *p == '<' || *p == '>') {
      fprintf(stderr, "get a %c\n", *p);
      token->ty = *p;
      token->input = p;
      addData(linked_list, token);
      ++p;
      continue;
    }

    // tokenize an integer
    if (isdigit(*p)) {
      fprintf(stderr, "get a number\n");
      token->ty = TK_NUM;
      token->input = p;
      // NOTE: strtol update the pointer that is specified in second parameter
      //       to the next character of last read
      long val = strtol(p, &p, 10);
      fprintf(stderr, "the geven value is %ld\n", val);
      token->val = val;
      addData(linked_list, token);
      continue;
    }

    error_at(user_input, p, "can not tokenize.");
  }

  fprintf(stderr, "finally, add EOF token\n");
  Token *token = (Token *)malloc(sizeof(Token));
  token->ty = TK_EOF;
  token->input = p;

  addData(linked_list, token);
  fprintf(stderr, "total %d tokens is generated\n", linked_list->data_num);

  fprintf(stderr, "tokenize finish\n");
}

// check token type, and if token is the same with the variable,
// increment pos (it is a global variable) and return true, otherwise return false
int consume (LinkedList *linked_list, int ty) {
  fprintf(stderr, "consume start\n");
  Token *token = getData(linked_list, 0);
  if (token->ty != ty) {
    fprintf(stderr, "got token is not expected type. consume finish\n");
    return 0;
  }
  removeData(linked_list, 0);
  fprintf(stderr, "got token is expected type. consume finish\n");
  return 1;
}
