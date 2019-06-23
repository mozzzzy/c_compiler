#include <stdio.h>    // fprintf
#include <stdlib.h>   // malloc
#include <string.h>   // strcmp

#include <linkedList.h>   // LinkedList

#include "error_handler.h"  // error
#include "node.h"
#include "tokenizer.h"      // Token data type

/**
 * functions
 */
// search ident from linkedlist
int search_ident (LinkedList *ident_list, char *ident_name) {
  int data_count = 0;
  for (data_count = 0; data_count < ident_list->data_num; ++data_count) {
    if (!strcmp((char *)getData(ident_list, data_count), ident_name)) {
      return data_count;
    }
  }
  return -1;
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

// create a node of identifier
Node *new_node_ident (char *name) {
  fprintf(stderr, "new_node_ident start\n");

  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;

  fprintf(stderr, "new_node_ident finish\n");
  return node;
}

// EBNF
// program    = stmt*
// stmt = expr ";" | "return" expr ";"
// expr       = assign
// assign     = equality ("=" assign)?
// equality   = relational ("==" relational | "!= relational)*
// relational = add ("<" add | "<=" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | unary)*
// unary      = ("+" | "-")? term
// term       = num | ident | "(" expr ")"

// creates nodes that form a program
// program = stmt*
void program (LinkedList *token_list, LinkedList *ident_list) {
  int stmt_count = 0;
  while (1) {
    // get a token from token_list
    Token *token = getData(token_list, 0);

    // if the token type is TK_EOF, break
    if (token->ty == TK_EOF) {
      break;
    }

    code[stmt_count++] = stmt(token_list, ident_list);
  }
  code[stmt_count] = NULL;
}

// creates nodes that form a stmt
// stmt       = expr ";"
Node *stmt (LinkedList *token_list, LinkedList *ident_list) {
  Node *node = NULL;

  if (consume(token_list, TK_RETURN)) {
    node = new_node(ND_RETURN, expr(token_list, ident_list), NULL);
  } else {
    node = expr(token_list, ident_list);
  }

  // next of expr should be ';'
  if (!consume(token_list, ';')) {
    error("stmt(): token is not ';'");
  }
  return node;
}

// creates nodes that form an expr
// expr       = assign
Node *expr(LinkedList *token_list, LinkedList *ident_list) {
  Node *node = assign(token_list, ident_list);
  return node;
}

// creates nodes that form a assign
// assign     = equality ("=" assign)?
Node *assign (LinkedList *token_list, LinkedList *ident_list) {
  Node *node = equality(token_list, ident_list);
  // if the next token is '='
  if (consume(token_list, '=')) {
    // create a node whose type is '='
    node = new_node('=', node, assign(token_list, ident_list));
  }
  return node;
}

// creates nodes that form an equality
// equality   = relational ("==" relational | "!=" relational)*
Node *equality (LinkedList *token_list, LinkedList *ident_list) {
  Node *node = relational(token_list, ident_list);
  for (;;) {
    // if target token is "=="
    if (consume(token_list, TK_EQ)) {
      // create a node whose type is TK_EQ
      // and whose left-hand side is the relational in the front
      // and whose right-hand size is the relational in the back
      node = new_node(TK_EQ, node, relational(token_list, ident_list));

    // the following lines are a repeat of the same thing
    } else if (consume(token_list, TK_NE)) {
      node = new_node(TK_NE, node, relational(token_list, ident_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a relational
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational (LinkedList *token_list, LinkedList *ident_list) {
  Node *node = add(token_list, ident_list);
  for (;;) {
    // if target token is <
    if (consume(token_list, '<')) {
      // create a node whose type is '<'
      // and whose left-hand side is the add in the front
      // and whose right-hand size is the add in the back
      node = new_node('<', node, add(token_list, ident_list));

    // the following lines are a repeat of the same thing
    } else if (consume(token_list, '>')) {
      node = new_node('<', node, add(token_list, ident_list));
    } else if (consume(token_list, TK_GE)) {
      node = new_node(TK_GE, node, add(token_list, ident_list));
    } else if (consume(token_list, TK_LE)) {
      node = new_node(TK_LE, node, add(token_list, ident_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a add
// add        = mul ("+" mul | "-" mul)*
Node *add(LinkedList *token_list, LinkedList *ident_list) {
  Node *node = mul(token_list, ident_list);
  for (;;) {
    // if target token is +
    if (consume(token_list, '+')) {
      // create a node whose type is '+'
      // and whose left-hand side is the mul in the front
      // and whose right-hand side is the mul in the back
      node = new_node('+', node, mul(token_list, ident_list));

    // the following lines are a repeat of the same thing
    } else if (consume(token_list, '-')) {
      node = new_node('-', node, mul(token_list, ident_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a mul
// mul        = unary ("*" unary | "/" unary)*
Node *mul (LinkedList *token_list, LinkedList *ident_list) {
  Node *node = unary(token_list, ident_list);
  for (;;) {
    // if the target token is '*'
    if (consume(token_list, '*')) {
      // create a node whose type is '*'
      // and whose left-hand side is the unary in the front
      // and whose right-hand side is the unary in the back
      node = new_node('*', node, unary(token_list, ident_list));

    // the following lines are a repeat of the same thing
    } else if (consume(token_list, '/')) {
      node = new_node('/', node, unary(token_list, ident_list));
    } else {
      return node;
    }
  }
}

// creates nodes that form a unary
// unary      = ("+" | "-")? term
Node *unary (LinkedList *token_list, LinkedList *ident_list) {
  if (consume(token_list, '+')) {
    return term(token_list, ident_list);
  }
  if (consume(token_list, '-')) {
    // if there is "-x",
    // replace to "0 - x"
    return new_node('-', new_node_num(0), term(token_list, ident_list));
  }
  return term(token_list, ident_list);
}

// creates nodes that form term
// term       = num | ident | "(" expr ")"
Node *term (LinkedList *token_list, LinkedList *ident_list) {
  // if the target token is '('
  if (consume(token_list, '(')) {
    // create a node of expression
    Node *node = expr(token_list, ident_list);

    if (!consume(token_list, ')')) {
      error("term(): ')' does not exist");
    }
    return node;
  }

  // if the target token is not '(', its type should be TK_NUM or TK_IDENT
  Token *token = getData(token_list, 0);
  if (token->ty == TK_NUM) {
    removeData(token_list, 0);
    return new_node_num(token->val);
  }
  if (token->ty == TK_IDENT) {
    removeData(token_list, 0);

    if (search_ident(ident_list, token->name) == -1) {
      addData(ident_list, token->name);
    }
    addData(ident_list, token->name);

    return new_node_ident(token->name);
  }

  error(
    "term(): target token is neither a left parenthesis '(' nor a number nor a identifier");

  return NULL;
}
