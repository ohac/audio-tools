#!/usr/bin/env ruby
require "./common.rb"

prog = [
  # initial stack: status note velocity
  # status:
  #   0x80 <= status < 0xf0: voice message
  #     0x8n: note off
  #     0x9n: note on
  #     0xan: pressure (after touch)
  #     0xbn: control change
  #     0xcn: program change
  #     0xdn: pressure (after touch)
  #     0xen: pitch wheel change
  #   0xf0, 0xf1, 0xf2, 0xf3, 0xf6: system message
  #   0xf8, 0xfa, 0xfb, 0xfc, 0xfe, 0xff: system realtime message
  #   < 0x80, 0xf4, 0xf5, 0xf7, 0xf9, 0xfd: invalid

  OP_3DUP, OP_DROP, OP_DROP, # duplicate status to top
  OP_16, OP_DIV, # top >>= 4
  OP_8, OP_SUB, # top -= 8
  OP_3, OP_JZ, # if note off then jump to else
    OP_DROP, OP_RETURN,
  # else
    OP_DROP, OP_DROP, OP_DROP, OP_DROP,
]

print_result(prog)
