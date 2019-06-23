#include <stdio.h>    // fprintf

#include <linkedList.h>     // LinkedList

#include "code_generator.h" // gen
#include "error_handler.h"  // error
#include "node.h"           // Node data type
#include "tokenizer.h"      // tokenize

char *USAGE = "Usage:\n"
              "  %s CODE\n";


int main (int argc, char **argv) {
  fprintf(stderr, "%s start\n", argv[0]);

  // if number of arguments is not 2, show usage and return 1
  if (argc != 2) {
    error(USAGE, argv[0]);
    return 1;
  }

  // source code
  char *user_input = argv[1];

  // create linked list
  fprintf(stderr, "create a linked list\n");
  LinkedList *token_list = initLinkedList();
  fprintf(stderr, "created successfully\n");

  // tokenize
  // NOTE: following function tokenizes user_input,
  //       and then put each token into the token_list.
  fprintf(stderr, "tokenize the geven program\n");
  tokenize(user_input, token_list);
  fprintf(stderr, "tokenized successfully\n");

  // create syntax tree of tokens and ident map.
  fprintf(stderr, "create a syntax tree\n");
  LinkedList *ident_list = initLinkedList();
  program(token_list, ident_list);
  fprintf(stderr, "created a syntax tree successfully\n");

  fprintf(stderr, "output compiled assembly\n\n");


  // now write assembly.
  // command to specify the syntax of assembly
  printf(".intel_syntax noprefix\n");
  // this means main function is in global scope, not file scope
  printf(".global main\n");
  // main function
  printf("main:\n");

  // prologue.
  // allocate space for 26 of variables.
  // first, push value of memory address that is contained in rbp register
  // to the stack top.
  // in x86-64 architecture, rbp register is used as bese register.
  // this points the start point of the function frame.
  printf("  push rbp\n");
  // second, set address that is contained in rsp to rbp.
  // this address is the top of the main's function frame.
  // function frame is the memory area that is allocated for each
  // function call.
  //   +------------------------------+
  //   |      ...                     |
  //   +------------------------------+
  //   | top of main function frame   | <- RBP, RSP
  //   +------------------------------+
  //   |      ...                     |
  //   |                              |
  //   |                              |
  //   |                              |
  //   +------------------------------+
  printf("  mov rbp, rsp\n");
  // finally, subtracts 208 (= 8 * 26) from rsp.
  // then rbp and rsp register point the following points.
  //   +------------------------------+
  //   |      ...                     |
  //   +------------------------------+
  //   | top of main function's frame | <- RBP
  //   +------------------------------+
  //   |                              |
  //   | space for 25 of variables.   |
  //   | (200 byte)                   |
  //   |                              |
  //   +------------------------------+
  //   |                              | <- RSP
  //   +------------------------------+
  //
  printf("  sub rsp, 208\n");   // 8 * 26

  // generate assembly from syntax tree
  int code_count = 0;
  for (code_count = 0; code[code_count]; code_count++) {
    if (gen(code[code_count], ident_list) == 1) {
      error("failed to generate assembly.\n");
    }

    // the result value should be on the top of the stack, so pop
    printf("  pop rax\n");
  }

  // return
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
