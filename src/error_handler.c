#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

#include <stdlib.h>   // exit
#include <stdarg.h>   // va_list
#include <stdio.h>    // fprintf

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

#endif  // ERROR_HANDLER_H_
