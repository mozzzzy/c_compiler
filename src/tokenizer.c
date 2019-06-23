#include <ctype.h>    // isspace
#include <stdio.h>    // fprintf
#include <stdlib.h>   // malloc
#include <string.h>   // strncmp

#include <linkedList.h>     // LinkedList

#include "error_handler.h"  // error_at
#include "tokenizer.h"

// helper function.
// check that specified char data is an alphabet or a number or an underscore or not
int is_alnum (char c) {
  return
    ('a' <= c && c <= 'z') ||
    ('A' <= c && c <= 'Z') ||
    ('0' <= c && c <= '9') ||
    (c == '_');
}

size_t get_name_len (char *p) {
  size_t name_len = 0;
  while (is_alnum(*p)) {
    ++name_len;
    ++p;
  }
  return name_len;
}

char *copy_name (char *p, size_t name_len) {
  char *name = (char *)malloc(name_len * sizeof(char));
  strncpy(name, p, name_len);
  return name;
}

// tokenize user input and put each token to the array
void tokenize (char *user_input, LinkedList *linked_list) {
  fprintf(stderr, "tokenize start\n");

  char *p = user_input;
  fprintf(stderr, "user_input is following codes\n");
  fprintf(stderr, "8< - - 8< - - 8< - - 8<\n");
  fprintf(stderr, "%s", p);
  fprintf(stderr, "\n8< - - 8< - - 8< - - 8<\n");

  // for each index of the array
  while (*p) {
    // skip a space
    if (isspace(*p)) {
      fprintf(stderr, "get a space\n");
      ++p;
      continue;
    }

    // create a new token
    Token *token = (Token *)malloc(sizeof(Token));
    // tokenize "return"
    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      token->ty = TK_RETURN;
      token->input = p;
      addData(linked_list, token);
      p += 6;
      continue;
    }

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

    // tokenize +, -, *, /, (, ), <, >, = ;
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
        || *p == '(' || *p == ')' || *p == '<' || *p == '>'
        || *p == '=' || *p == ';') {
      fprintf(stderr, "get a \"%c\"\n", *p);
      token->ty = *p;
      token->input = p;
      addData(linked_list, token);
      ++p;
      continue;
    }

    // tokenize a token
    if ('a' <= *p && *p <= 'z') {
      fprintf(stderr, "get an identifier \"%c\"\n", *p);
      size_t name_len = get_name_len(p);
      token->ty = TK_IDENT;
      token->name = copy_name(p, name_len);
      printf("### token->name = \"%s\"\n", token->name);
      token->input = p;
      addData(linked_list, token);
      p += name_len;
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

  // if token type is not the specified one, return 0
  if (token->ty != ty) {
    fprintf(stderr, "got token is not expected type. consume finish\n");
    return 0;
  }

  // if token type is the specified one, remove the token and return 1
  removeData(linked_list, 0);
  fprintf(stderr, "got token is expected type. consume finish\n");
  return 1;
}
