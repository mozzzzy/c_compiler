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
  LinkedList *linked_list = initLinkedList();
  fprintf(stderr, "created successfully\n");

  // tokenize
  // NOTE: following function tokenizes user_input,
  //       and then put each token into the array "tokens".
  //       tokens is an global variable.
  fprintf(stderr, "tokenize the geven program\n");
  tokenize(user_input, linked_list);
  fprintf(stderr, "tokenized successfully\n");

  // create syntax tree of tokens
  fprintf(stderr, "create a syntax tree\n");
  Node *node = expr(linked_list);
  fprintf(stderr, "created a syntax tree successfully\n");

  fprintf(stderr, "output compiled assembly\n\n");

  // command to specify the syntax of assembly
  printf(".intel_syntax noprefix\n");
  // this means main function is in global scope, not file scope
  printf(".global main\n");
  // main function
  printf("main:\n");

  // generate assembly from syntax tree
  gen(node);

  // the result value should be on the top of the stack, so pop
  printf("  pop rax\n");

  // return
  printf("  ret\n");
  return 0;
}
