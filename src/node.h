#ifndef NODE_H_
#define NODE_H_

#include <linkedList.h>   // LinkedList

/**
 * data types
 */

// number to specify the node type
enum {
  ND_NUM = 256,   // node type of integer
  ND_RETURN,      // node type of return
  ND_IDENT,       // node type of identifier
};

// type of a node of syntax tree
typedef struct Node {
  int ty;             // node type
  struct Node *lhs;   // left-hand side
  struct Node *rhs;   // right-hand side
  int val;            // the value of token.
                      // NOTE: this variable is used only if ty == ND_NUM
  char *name;         // the value of token.
                      // NOTE: this variable is used only if ty == ND_IDENT
} Node;


/**
 * variables
 */

Node *code[100];

/**
 * functions
 */

// create a node of binary operator
Node *new_node (int ty, Node *lhs, Node *rhs);

// create a node of number
Node *new_node_num (int val);

// EBNF
// program = stmt*
// stmt = expr ";" | "return" expr ";"
// expr = assign
// assign = equality ("=" assign)?
// equality = relational ("==" relational | "!= relational)*
// relational = add ("<" add | "<=" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | unary)*
// unary = ("+" | "-")? term
// term = num | ident | "(" expr ")"
void program(LinkedList *token_list, LinkedList *ident_list);
Node *stmt(LinkedList *token_list, LinkedList *ident_list);
Node *expr(LinkedList *token_list, LinkedList *ident_list);
Node *assign(LinkedList *token_list, LinkedList *ident_list);
Node *equality(LinkedList *token_list, LinkedList *ident_list);
Node *relational(LinkedList *token_list, LinkedList *ident_list);
Node *add(LinkedList *token_list, LinkedList *ident_list);
Node *mul(LinkedList *token_list, LinkedList *ident_list);
Node *unary(LinkedList *token_list, LinkedList *ident_list);
Node *term(LinkedList *token_list, LinkedList *ident_list);

#endif  // NODE_H_
