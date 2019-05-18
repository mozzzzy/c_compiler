#include <stdio.h>
#include <stdlib.h>

char *USAGE = "Usage:\n"
              "  9cc CODE\n";

int main (int argc, char **argv) {
  // if number of arguments is not 2, show usage and return 1
  if (argc != 2) {
    fprintf(stderr, USAGE);
    return 1;
  }

  // get source code from argv[1]
  char *p = argv[1];

  // command to specify the syntax of assembly
  printf(".intel_syntax noprefix\n");
  // this means main function is in global scope, not file scope
  printf(".global main\n");

  // main function
  printf("main:\n");
  // move the first integer that is specified in argv[1] to rax.
  // RAX is a register. the data in RAX when the function returns
  // is used to return value of the function.
  //
  // NOTE: strtol update the pointer that is specified in second parameter
  //       to the next character of last read
  printf("  mov rax, %ld\n", strtol(p, &p, 10));

  while (*p) {
    if (*p == '+') {
      ++p;
      // adds the integer to the right of '+' to rax.
      printf("  add rax, %ld\n", strtol(p, &p, 10));
      continue;
    }
    if (*p == '-') {
      ++p;
      // substracts the integer to the right of '+' from rax.
      printf("  sub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    fprintf(stderr, "unexpected character: '%c'\n", *p);
    return 1;
  }

  // return
  printf("  ret\n");
  return 0;
}
