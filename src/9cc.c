#include <ctype.h>    // isspace
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// tokenized results (= tokens)
// NOTE: we can't tokenize the program which has more than 100 tokens.
Token tokens[100];

// specify the position of the target token of the above array
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

    // tokenize ==
    if (strncmp(p, "==", 2) == 0) {
      tokens[i].ty = TK_EQ;
      ++i;
      p += 2;
      continue;
    }

    // tokenize !=
    if (strncmp(p, "!=", 2) == 0) {
      tokens[i].ty = TK_NE;
      ++i;
      p += 2;
      continue;
    }

    // tokenize <=
    if (strncmp(p, "<=", 2) == 0) {
      tokens[i].ty = TK_LE;
      ++i;
      p += 2;
      continue;
    }

    // tokenize <=
    if (strncmp(p, ">=", 2) == 0) {
      tokens[i].ty = TK_GE;
      ++i;
      p += 2;
      continue;
    }

    // tokenize +, -, *, /, (, ), <, >
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
        || *p == '(' || *p == ')' || *p == '<' || *p == '>') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      ++i;
      ++p;
      continue;
    }

    // tokenize an integer
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
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
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
Node *expr () {
  Node *node = equality();
  return node;
}

// creates nodes that form an equality
// equality   = relational ("==" relational | "!=" relational)*
Node *equality () {
  Node *node = relational();
  for (;;) {
    // if target token is "=="
    if (consume(TK_EQ)) {
      // create a node whose type is TK_EQ
      // and whose left-hand side is the relational in the front
      // and whose right-hand size is the relational in the back
      node = new_node(TK_EQ, node, relational());

    // the following lines are a repeat of the same thing
    } else if (consume(TK_NE)) {
      node = new_node(TK_NE, node, relational());
    } else {
      return node;
    }
  }
}

// creates nodes that form a relational
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational () {
  Node *node = add();
  for (;;) {
    // if target token is <
    if (consume('<')) {
      // create a node whose type is '<'
      // and whose left-hand side is the add in the front
      // and whose right-hand size is the add in the back
      node = new_node('<', node, add());

    // the following lines are a repeat of the same thing
    } else if (consume('>')) {
      node = new_node('<', node, add());
    } else if (consume(TK_GE)) {
      node = new_node(TK_GE, node, add());
    } else if (consume(TK_LE)) {
      node = new_node(TK_LE, node, add());
    } else {
      return node;
    }
  }
}

// creates nodes that form a add
// add        = mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();
  for (;;) {
    // if target token is +
    if (consume('+')) {
      // create a node whose type is '+'
      // and whose left-hand side is the mul in the front
      // and whose right-hand side is the mul in the back
      node = new_node('+', node, mul());

    // the following lines are a repeat of the same thing
    } else if (consume('-')) {
      node = new_node('-', node, mul());
    } else {
      return node;
    }
  }
}

// creates nodes that form a mul
// mul        = unary ("*" unary | "/" unary)*
Node *mul () {
  Node *node = unary();
  for (;;) {
    // if the target token is '*'
    if (consume('*')) {
      // create a node whose type is '*'
      // and whose left-hand side is the unary in the front
      // and whose right-hand side is the unary in the back
      node = new_node('*', node, unary());

    // the following lines are a repeat of the same thing
    } else if (consume('/')) {
      node = new_node('/', node, unary());
    } else {
      return node;
    }
  }
}

// creates nodes that form a unary
// unary      = ("+" | "-")? term
Node *unary () {
  if (consume('+')) {
    return term();
  }
  if (consume('-')) {
    // if there is "-x",
    // replace to "0 - x"
    return new_node('-', new_node_num(0), term());
  }
  return term();
}

// creates nodes that form term
// term       = num | "(" expr ")"
Node *term () {
  // if the target token is '('
  if (consume('(')) {
    // create a node of expression
    Node *node = expr();

    if (!consume(')')) {
      error("term(): ')' does not exist");
    }
    return node;
  }

  // if the target token is not '(', its type should be TK_NUM
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
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
