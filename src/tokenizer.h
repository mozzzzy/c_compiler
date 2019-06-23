#include <linkedList.h>   // LinkedList

/**
 * data types
 */

// number to specify the token type
enum {
  TK_NUM = 256,   // integer token
  TK_RETURN,      // return token          return
  TK_IDENT,       // identifier            a, b, c
  TK_EQ,          // equal                 ==
  TK_NE,          // not qeual             !=
  TK_LE,          // less than or equal    <=
  TK_GE,          // greater than or equal >=
  TK_EOF          // end of input token
};

// data type of a token
typedef struct {
  int ty;       // token type
  int val;      // if ty == TK_NUM, val is token's value
  char *name;   // it ty == TK_IDENT, name is token's name. this name is formed
                // by multiple characters.
  char *input;  // token string (for error messaging)
} Token;


/**
 * functions
 */

// tokenize user input and put each token to the array
void tokenize (char *user_input, LinkedList *linked_list);

// check token type, and if token is the same with the variable,
// increment pos (it is a global variable) and return true, otherwise return false
int consume (LinkedList *linked_list, int ty);
