#include <stdio.h>    // printf
#include <string.h>   // strcmp

#include <linkedList.h>

#include "error_handler.h"
#include "code_generator.h"
#include "node.h"           // Node data type
#include "tokenizer.h"      // TK_EQ, TK_NE ...

/**
 * functions
 */

int gen_lval (Node *node, LinkedList *ident_list) {
  if (node->ty != ND_IDENT) {
    error("gen_lval(): node type is not ND_IDENT");
  }

  // get index of ident_list
  int ident_index = -1;
  int ident_count = 0;
  for (ident_count = 0; ident_count < ident_list->data_num; ++ident_count) {
    if (!strcmp(node->name, (char *)getData(ident_list, ident_count))) {
      ident_index = ident_count;
      break;
    }
  }

  // if ident->name does not exist in ident_list,
  // return with 1. this means error.
  if (ident_index == -1) {
    return 1;
  }

  int offset = (ident_index + 1) * 8;
  // sets address that is contained in rbp to rax.
  // rbp contains the address that is the top of function frame.
  //   +-----------------------------+
  //   |  ...                        |
  //   +-----------------------------+
  //   | function frame              | <- RAX, RBP
  //   |                             |
  //   |                             |
  //   |                             |
  //   |  ...                        |
  //   |                             | <- RSP
  //   |  ...                        |
  //   +-----------------------------+
  printf("  mov rax, rbp\n");
  // subtracts offset from rax.
  // this means that we move rax to the address that is calculated by "rbp - offset"
  //   +-----------------------------+
  //   |  ...                        |
  //   +-----------------------------+
  //   | function frame              | <- RBP
  //   |                             |
  //   |                             | <- RAX (= RBP - offset)
  //   |                             |
  //   |  ...                        |
  //   |                             | <- RSP
  //   |  ...                        |
  //   +-----------------------------+
  printf("  sub rax, %d\n", offset);
  // push value of rax to the stack
  //   +-----------------------------+
  //   |  ...                        |
  //   +-----------------------------+
  //   | function frame              | <- RBP
  //   |                             |
  //   |                             | <- RAX (= RBP - offset)
  //   |                             |
  //   |  ...                        |
  //   |                             | <- RSP
  //   | value of RAX(= RBP - offset)|
  //   |  ...                        |
  //   +-----------------------------+
  printf("  push rax\n");

  return 0;
}

// generate assembly from a syntax tree
int gen (Node *node, LinkedList* ident_list) {
  // if node type is ND_RETURN
  if (node->ty == ND_RETURN) {
    // generate assembly of the expr that is at next of the return
    if (gen(node->lhs, ident_list) == 1) {
      return 1;
    }

    // above expr push the result value on the top of the stack.
    // so pop it and save into rax.
    printf("  pop rax\n");
    // save the value of rbp to rsp
    printf("  mov rsp, rbp\n");
    // pop to rbp. the popped data is the previous value of rbp
    printf("  pop rbp\n");
    // return
    printf("  ret\n");
    return 0;
  }

  // if the node type is ND_NUM
  if (node->ty == ND_NUM) {
    // push node's value to the stack top.
    // this command is the same with following commands.
    //   ```
    //   mov rax, [rsp]   # -> load the saved value of the memory area
    //                    #    whose address is saved in RSP to RAX.
    //   add rsp, 8       # -> add 8 to RSP
    //   ```
    // before and after this operation,
    // RSP register point the following area.
    //  [Before]
    //   +-----------------------------+
    //   |  ...                        |
    //   +-----------------------------+
    //   | function frame              |
    //   |                             |
    //   |  ...                        |
    //   |                             |
    //   |                             |  <- RSP. end of the function frame
    //   +-----------------------------+
    //  [After]
    //   +-----------------------------+
    //   |  ...                        |
    //   +-----------------------------+
    //   | function frame              |
    //   |                             |
    //   |  ...                        |
    //   |                             |  <- RSP
    //   +-----------------------------+
    //   | pushed number               |  <- end of the function frame
    //   +-----------------------------+
    //
    printf("  push %d\n", node->val);
    return 0;
  }

  // if the node type is ND_IDENT
  if (node->ty == ND_IDENT) {
    // push the address of the area which is allocated for the variable.
    if (gen_lval(node, ident_list) == 1) {
      return 1;
    }
    // pop the address to rax.
    printf("  pop rax\n");
    // load value from saved address in rax, and save the value into rax
    printf("  mov rax, [rax]\n");
    // push the value saved in rax into stack
    printf("  push rax\n");
    return 0;
  }

  // if the node type is '='
  if (node->ty == '=') {
    // push the address of the area which is allocated for the left-hand side variable
    if (gen_lval(node->lhs, ident_list) == 1) {
      return 1;
    }
    // push the value of the right-hand side node
    if (gen(node->rhs, ident_list) == 1) {
      return 1;
    }

    // pop the value to rdi
    printf("  pop rdi\n");
    // pop the address of the area for the variable to rax
    printf("  pop rax\n");
    // store the value saved in rdi to the address saved in rax
    printf("  mov [rax], rdi\n");
    // push the value of rdi
    printf("  push rdi\n");
    return 0;
  }

  // NOTE: if the node is not number and identifier and '=', reach following lines.
  //       i.e. +, -, *, /, ==, !=, <, >, <=, >=

  // push the value of the left-hand and right-hand side nodes
  if (gen(node->lhs, ident_list) == 1) {
    return 1;
  }
  if (gen(node->rhs, ident_list) == 1) {
    return 1;
  }

  // pop value of the right-hand side node to rdi
  printf("  pop rdi\n");
  // pop value of the left-hand side node to rax
  printf("  pop rax\n");

  switch (node->ty) {
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
    break;
  case TK_EQ:
    // compare rax and rdi.
    // on x86-64 architecture, the result of comparison is set to FLAGS register.
    // this register has some bits to contains the current state of the processor.
    printf("  cmp rax, rdi\n");
    // "sete" set 1 to al when previous cmp result is the same.
    // register AL is lower 8 bits of RAX.
    printf("  sete al\n");
    // when sete update AL value, RAX is updated too. but the upper 8 bits stay.
    // to zero clear the upper 8 bits of RAX, execute movzb.
    printf("  movzb rax, al\n");
    break;
  case TK_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case TK_GE:
    printf("  cmp rdi, rax\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case '>':
    printf("  cmp rdi, rax\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case TK_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case '<':
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
  return 0;
}
