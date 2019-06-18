#include <stdio.h>    // fprintf
#include <stdlib.h>   // malloc

#include <linkedList.h>   // LinkedList

#include "error_handler.h"  // error
#include "node.h"
#include "tokenizer.h"      // Token data type

/**
 * functions
 */

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
Node *new_node_ident (char name) {
  fprintf(stderr, "new_node_ident start\n");

  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;

  fprintf(stderr, "new_node_ident finish\n");
  return node;
}

// EBNF
// program    = stmt*
// stmt       = expr ";"
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
void program (LinkedList *linked_list) {
  int stmt_count = 0;
  while (1) {
    // get a token from linked_list
    Token *token = getData(linked_list, 0);

    // if the token type is TK_EOF, break
    if (token->ty == TK_EOF) {
      break;
    }

    code[stmt_count++] = stmt(linked_list);
  }
  code[stmt_count] = NULL;
}

// creates nodes that form a stmt
// stmt       = expr ";"
Node *stmt (LinkedList *linked_list) {
  Node *node = NULL;

  if (consume(linked_list, TK_RETURN)) {
    node = new_node(ND_RETURN, expr(linked_list), NULL);
  } else {
    node = expr(linked_list);
  }

  // next of expr should be ';'
  if (!consume(linked_list, ';')) {
    error("stmt(): token is not ';'");
  }
  return node;
}

// creates nodes that form an expr
// expr       = assign
Node *expr(LinkedList *linked_list) {
  Node *node = assign(linked_list);
  return node;
}

// creates nodes that form a assign
// assign     = equality ("=" assign)?
Node *assign (LinkedList *linked_list) {
  Node *node = equality(linked_list);
  // if the next token is '='
  if (consume(linked_list, '=')) {
    // create a node whose type is '='
    node = new_node('=', node, assign(linked_list));
  }
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
// term       = num | ident | "(" expr ")"
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

  // if the target token is not '(', its type should be TK_NUM or TK_IDENT
  Token *token = getData(linked_list, 0);
  if (token->ty == TK_NUM) {
    removeData(linked_list, 0);
    return new_node_num(token->val);
  }
  if (token->ty == TK_IDENT) {
    removeData(linked_list, 0);
    return new_node_ident(*(token->input));
  }

  error(
    "term(): target token is neither a left parenthesis '(' nor a number nor a identifier");

  return NULL;
}
