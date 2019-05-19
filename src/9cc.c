#include <ctype.h>    // isspace
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * data tipes
 */
// number to specify the type of token
enum {
  TK_NUM = 256,   // integer token
  TK_EOF,         // end of input token
};

// type of a token
typedef struct {
  int ty;       // type of token
  int val;      // if ty == TK_NUM, val is token's value
  char *input;  // token string (for error messaging)
} Token;


/**
 * variables
 */
char *USAGE = "Usage:\n"
              "  %s CODE\n";

// tokenized results (= token)
// NOTE: we can't tokenize the program which has more than 100 tokens.
Token tokens[100];


/**
 * functions
 */
// print error message
// NOTE: this function take the same parameters with printf
void error (char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  exit(1);
}

// print error point
void error_at (char *user_input, char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  // print spaces pos times
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// tokenize user input and put each token to the array
void tokenize (char *user_input) {
  char *p = user_input;

  // index of the array
  int i = 0;
  while (*p) {
    // skip a space
    if (isspace(*p)) {
      ++p;
      continue;
    }

    // tokenize + and -
    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      ++i;
      ++p;
      continue;
    }

    // tokenize a integer
    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      // NOTE: strtol update the pointer that is specified in second parameter
      //       to the next character of last read
      tokens[i].val = strtol(p, &p, 10);
      ++i;
      continue;
    }

    error_at(user_input, p, "can not tokenize.");
  }
  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main (int argc, char **argv) {
  // if number of arguments is not 2, show usage and return 1
  if (argc != 2) {
    error(USAGE, argv[0]);
    return 1;
  }

  // source code
  char *user_input = argv[1];
  // tokenize
  tokenize(user_input);

  // command to specify the syntax of assembly
  printf(".intel_syntax noprefix\n");
  // this means main function is in global scope, not file scope
  printf(".global main\n");
  // main function
  printf("main:\n");

  // the first character of input program should be an integer
  if (tokens[0].ty != TK_NUM) {
    error_at(user_input, tokens[0].input, "first token is not a number");
  }

  // move the first integer that is specified in argv[1] to rax.
  // RAX is a register. the data in RAX when the function returns
  // is used to return value of the function.
  printf("  mov rax, %d\n", tokens[0].val);

  // index of token array
  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      ++i;
      if (tokens[i].ty != TK_NUM) {
        error_at(user_input, tokens[i].input, "token after '+' should be a number");
      }
      // adds the integer to the right of '+' to rax.
      printf("  add rax, %d\n", tokens[i].val);
      ++i;
      continue;
    }

    if (tokens[i].ty == '-') {
      ++i;
      if (tokens[i].ty != TK_NUM) {
        error_at(user_input, tokens[i].input, "token after '-' should be a number");
      }
      // substracts the integer to the right of '+' from rax.
      printf("  sub rax, %ld\n", tokens[i].val);
      ++i;
      continue;
    }

    error_at(user_input, tokens[i].input, "unexpected token");
  }

  // return
  printf("  ret\n");
  return 0;
}
