#!/bin/bash

# helper function
try() {
  expected="$1"
  input="$2"

  # create assembly file
  export LD_LIBRARY_PATH=`pwd`/../src/linkedlist/src:${LD_LIBRARY_PATH} \
  && ../src/9cc "$input" > tmp.s
  # create executable file from assembly file
  gcc -o tmp tmp.s
  # execute
  ./tmp
  # get result code
  actual="$?"

  # check whether expected code and actual code are the same or not
  # if expected and actual are the same
  if [ "$actual" = "$expected" ]; then
    echo "OK: $input => $actual"
  # if not
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

# test main
# check that 9cc can compile the expression that
# has a number
try 0 "0;"
try 42 "42;"
# check that 9cc can compile the expression that
# has + or -
try 21 "5+20-4;"
# check that 9cc can compile the expression that
# has spaces
try 21 "5 + 20 - 4;"
try 21 "5+ 20 -4;"
try 21 " 5 + 20 - 4 ;"
# check that 9cc can compile the expression that
# has * or or / or ( )
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
# check that 9cc can compile the expression that
# has unary operator - or +
try 8 "-4 + 12;"
try 20 "-4 * -5;"
# check that 9cc can compile the expression that has ==
try 1 "111 == 111;"
try 0 "13 == 5;"
try 1 "111 == 111 == 1;"
try 0 "111 == 115 == 1;"
try 0 "1 == (115 == 111);"
# check that 9cc can compile the expression that has !=
try 0 "111 != 111;"
try 1 "13 != 5;"
try 1 "111 != 111 != 1;"
try 0 "111 != 115 != 1;"
# check that 9cc can compile the expression that has <=
try 1 "10 <= 123;"
try 1 "123 <= 123;"
try 0 "123 <= 123 <= 0;"
# check that 9cc can compile the expression that has <=
try 1 "10 < 123;"
try 0 "123 < 123;"
try 0 "123 < 123 < 0;"
# check that 9cc can compile the expression that has more than 100 tokens
try 225 "1+2+3+4+5+6+7+8+9+0+1+2+3+4+5+6+7+8+9+0+1+2+3+4+5+6+7+8+9+0+1+2+3+4+5+6+7+8+9+0+1+2+3+4+5+6+7+8+9;"
# check that 9cc can compile the program that has variable
# whose name is formed by one character
try 10 "a = 10;"
try 13 "a = 10; a + 3;"
try 15 "a = 10; b = 5; a + b;"
# check that 9cc can compile the program that has return statement
try 4 "return 4;"
try 4 "return 4; 1 + 1;"
try 4 "return 4; return (1 + 1);"
try 4 "a = 4; return a;"
# check that 9cc can compile the program that has variable
# whose name is formed by multi character
try 10 "var1 = 8; var2 = 2; return var1 + var2;"
echo ALL OK
