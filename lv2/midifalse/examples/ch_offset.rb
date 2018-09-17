#!/usr/bin/env ruby
require "./common.rb"

prog = [
  OP_ROT, # status to top
  OP_1, OP_ADD, # st += 1 (ch++)
  OP_ROT,
  OP_ROT,
]

print_result(prog)
