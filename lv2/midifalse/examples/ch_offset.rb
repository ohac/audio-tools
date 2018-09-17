#!/usr/bin/env ruby
require "./common.rb"

prog = [
  S_OP_ROT, # status to top
  S_OP_1, S_OP_ADD, # st += 1 (ch++)
  S_OP_ROT,
  S_OP_ROT,
]

print_result(prog)
