#!/bin/bash

# helper function
try() {
  expected="$1"
  input="$2"

  # create assembly file
  ../src/9cc "$input" > tmp.s
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
try 0 0
try 42 42
try 21 "5+20-4"
echo ALL OK
