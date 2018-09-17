#!/usr/bin/env ruby
require "./common.rb"

prog = [
  # initial stack: status note velocity
  OP_TUCK, # duplicate note to top of stack
  # if note < 121
  OP_16, OP_8, OP_MUL, OP_7, OP_SUB, OP_LESSTHAN, OP_3, OP_JNZ,
    OP_DROP, OP_RETURN, # thru
  # else
    OP_DROP, OP_3DUP, OP_SWAP, OP_7, OP_ADD, OP_SWAP, # add 5th
]

print_result(prog)
