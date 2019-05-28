#include <stdio.h>    // printf

#include "code_generator.h"
#include "node.h"           // Node data type
#include "tokenizer.h"      // TK_EQ, TK_NE ...

/**
 * functions
 */

// generate assembly from a syntax tree
void gen (Node *node) {
  // if top node of the syntax tree is ND_NUM,
  // the provided expression is only a number
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
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
}
