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

// number to specify the type of node
enum {
  ND_NUM = 256,   // node type of integer
};

// type of a token
typedef struct {
  int ty;       // type of token
  int val;      // if ty == TK_NUM, val is token's value
  char *input;  // token string (for error messaging)
} Token;

// type of a node of syntax tree
typedef struct Node {
  int ty;
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

// tokenized results (= token)
// NOTE: we can't tokenize the program which has more than 100 tokens.
Token tokens[100];

int pos = 0;

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
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
        || *p == '(' || *p == ')') {
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

// check token type, and if token is the same with the variable,
// increment pos (it is a global variable) and return true, otherwise return false
int consume (int ty) {
  if (tokens[pos].ty != ty) {
    return 0;
  }
  ++pos;
  return 1;
}

// create a node of binary operator
Node *new_node (int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// create a node of number
Node *new_node_num (int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

// function prototypes
Node *expr (char *user_input);
Node *mul (char *user_input);
Node *term (char *user_input);

// create syntax tree of an expression.
// an expression consists of the first multi-value and
// zero or more "+ multi-value" or "- multi-value".
// in other words in EBNF,
//   expr = mul ("+" mul | "-" mul)*
Node *expr (char *user_input) {
  Node *node = mul(user_input);

  for (;;) {
    if (consume('+')) {
      node = new_node('+', node, mul(user_input));
    } else if (consume('-')) {
      node = new_node('-', node, mul(user_input));
    } else {
      return node;
    }
  }
}

// create syntax tree of a multi-value.
// a multi-value consists of the first term and
// zero or more "* term" or "/ term".
// in other words in EBNF,
//   mul = term ("*" term | "/" term)*
Node *mul (char *user_input) {
  Node *node = term(user_input);

  for (;;) {
    // if the target token is '*', create a node of '*'
    if (consume('*')) {
      node = new_node('*', node, term(user_input));

    // if the target token is '/', create a node of '/'
    } else if (consume('/')) {
      node = new_node('/', node, term(user_input));

    } else {
      return node;
    }
  }
}

// create syntax tree of a term.
// a term consists of a number or "(" expr ")".
// in other words in EBNF,
//   term = number | "(" expr ")"
Node *term (char *user_input) {
  // if the target token is '('
  if (consume('(')) {
    // create a node of expression
    Node *node = expr(user_input);

    if (!consume(')')) {
      error_at(
        user_input, tokens[pos].input, "')' does not exist");
    }
    return node;
  }

  // if the target token is not '(', its type should be TK_NUM
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }

  error_at(
    user_input, tokens[pos].input,
    "target token is neither a left parenthesis '(' nor a number");
}

// generate assembly from syntax tree
void gen (Node *node) {
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
  }

  printf("  push rax\n");
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
  // NOTE: following function tokenizes user_input,
  //       and then put each token into the array "tokens".
  //       tokens is an global variable.
  tokenize(user_input);

  // create syntax tree of tokens
  Node *node = expr(user_input);

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
