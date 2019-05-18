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

  // command to specify the syntax of assembly
  printf(".intel_syntax noprefix\n");
  // this means main function is in global scope, not file scope
  printf(".global main\n");

  // main function
  printf("main:\n");
  // move integer that is specified in argv[1] to rax.
  // RAX is a register. the data in RAX when the function returns
  // is used to return value of the function
  printf("  mov rax, %d\n", atoi(argv[1]));
  // return
  printf("  ret\n");
  return 0;
}
