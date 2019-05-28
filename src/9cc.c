#include <ctype.h>    // isspace
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linkedList.h>

/**
 * data tipes
 */
// number to specify the token type
enum {
  TK_NUM = 256,   // integer token
  TK_EQ,          // equal                 ==
  TK_NE,          // not qeual             !=
  TK_LE,          // less than or equal    <=
  TK_GE,          // greater than or equal >=
  TK_EOF          // end of input token
};

// number to specify the node type
enum {
  ND_NUM = 256,   // node type of integer
};

// data type of a token
typedef struct {
  int ty;       // token type
  int val;      // if ty == TK_NUM, val is token's value
  char *input;  // token string (for error messaging)
} Token;

// type of a node of syntax tree
typedef struct Node {
  int ty;             // node type
  struct Node *lhs;   // left-hand side
  struct Node *rhs;   // right-hand side
  int val;            // the value of token.
                      // NOTE: this variable is used only if ty == ND_NUM
} Node;


/**
 * variables
 */
char *USAGE = "Usage:\n"
              "  %s CODE\n";

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

// create a node of binary operator
Node *new_node (int ty, Node *lhs, Node *rhs) {
  fprintf(stderr, "new_node start\n");

  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;

  fprintf(stderr, "new_node finish\n");
  return node;
}

// create a node of number
Node *new_node_num (int val) {
  fprintf(stderr, "new_node_num start\n");

  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;

  fprintf(stderr, "new_node_num finish\n");
  return node;
}

// function prototypes
Node *expr(LinkedList *linked_list);
Node *equality(LinkedList *linked_list);
Node *relational(LinkedList *linked_list);
Node *add(LinkedList *linked_list);
Node *mul(LinkedList *linked_list);
Node *unary(LinkedList *linked_list);
Node *term(LinkedList *linked_list);
// EBNF
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? term
// term       = num | "(" expr ")"


// creates nodes that form an expr
// expr       = equality
Node *expr (LinkedList *linked_list) {
  Node *node = equality(linked_list);
  return node;
}

// creates nodes that form an equality
// equality   = relational ("==" relational | "!=" relational)*
Node *equality (LinkedList *linked_list) {
  Node *node = relational(linked_list);
  for (;;) {
    // if target token is "=="
    if (consume(linked_list, TK_EQ)) {
      // create a node whose type is TK_EQ
      // and whose left-hand side is the relational in the front
      // and whose right-hand size is the relational in the back
      node = new_node(TK_EQ, node, relational(linked_list));

    // the following lines are a repeat of the same thing
    } else if (consume(linked_list, TK_NE)) {
      node = new_node(TK_NE, node, relational(linked_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a relational
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational (LinkedList *linked_list) {
  Node *node = add(linked_list);
  for (;;) {
    // if target token is <
    if (consume(linked_list, '<')) {
      // create a node whose type is '<'
      // and whose left-hand side is the add in the front
      // and whose right-hand size is the add in the back
      node = new_node('<', node, add(linked_list));

    // the following lines are a repeat of the same thing
    } else if (consume(linked_list, '>')) {
      node = new_node('<', node, add(linked_list));
    } else if (consume(linked_list, TK_GE)) {
      node = new_node(TK_GE, node, add(linked_list));
    } else if (consume(linked_list, TK_LE)) {
      node = new_node(TK_LE, node, add(linked_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a add
// add        = mul ("+" mul | "-" mul)*
Node *add(LinkedList *linked_list) {
  Node *node = mul(linked_list);
  for (;;) {
    // if target token is +
    if (consume(linked_list, '+')) {
      // create a node whose type is '+'
      // and whose left-hand side is the mul in the front
      // and whose right-hand side is the mul in the back
      node = new_node('+', node, mul(linked_list));

    // the following lines are a repeat of the same thing
    } else if (consume(linked_list, '-')) {
      node = new_node('-', node, mul(linked_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a mul
// mul        = unary ("*" unary | "/" unary)*
Node *mul (LinkedList *linked_list) {
  Node *node = unary(linked_list);
  for (;;) {
    // if the target token is '*'
    if (consume(linked_list, '*')) {
      // create a node whose type is '*'
      // and whose left-hand side is the unary in the front
      // and whose right-hand side is the unary in the back
      node = new_node('*', node, unary(linked_list));

    // the following lines are a repeat of the same thing
    } else if (consume(linked_list, '/')) {
      node = new_node('/', node, unary(linked_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a unary
// unary      = ("+" | "-")? term
Node *unary (LinkedList *linked_list) {
  if (consume(linked_list, '+')) {
    return term(linked_list);
  }
  if (consume(linked_list, '-')) {
    // if there is "-x",
    // replace to "0 - x"
    return new_node('-', new_node_num(0), term(linked_list));
  }
  return term(linked_list);
}

// creates nodes that form term
// term       = num | "(" expr ")"
Node *term (LinkedList *linked_list) {
  // if the target token is '('
  if (consume(linked_list, '(')) {
    // create a node of expression
    Node *node = expr(linked_list);

    if (!consume(linked_list, ')')) {
      error("term(): ')' does not exist");
    }
    return node;
  }

  // if the target token is not '(', its type should be TK_NUM
  Token *token = getData(linked_list, 0);
  if (token->ty == TK_NUM) {
    removeData(linked_list, 0);
    return new_node_num(token->val);
  }

  error(
    "term(): target token is neither a left parenthesis '(' nor a number");
}

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
