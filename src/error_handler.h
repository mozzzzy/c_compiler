#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

// print error message
// NOTE: this function take the same parameters with printf
void error (char *fmt, ...);

// print error point
void error_at (char *user_input, char *loc, char *msg);

#endif  // ERROR_HANDLER_H_
