#!/usr/bin/env ruby
require "./common.rb"

prog = [
  # initial stack: status note velocity
  S_OP_TUCK, # duplicate note to top of stack
  # if note < 121
  S_OP_16, S_OP_8, S_OP_MUL, S_OP_7, S_OP_SUB, S_OP_LESSTHAN, S_OP_3, S_OP_JNZ,
    S_OP_DROP, S_OP_RETURN, # thru
  # else
    S_OP_DROP, S_OP_3DUP, S_OP_SWAP, S_OP_7, S_OP_ADD, S_OP_SWAP, # add 5th
    S_OP_RETURN
]

print_result(prog)
