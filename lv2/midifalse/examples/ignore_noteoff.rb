#!/usr/bin/env ruby
require "./common.rb"

program = [
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

  S_OP_3DUP, S_OP_DROP, S_OP_DROP, # duplicate status to top
  S_OP_16, S_OP_DIV, # top >>= 4
  S_OP_8, S_OP_SUB, # top -= 8
  S_OP_3, S_OP_JZ, # if note off then jump to else
    S_OP_DROP, S_OP_RETURN,
  # else
    S_OP_DROP, S_OP_DROP, S_OP_DROP, S_OP_DROP, S_OP_RETURN,
]

result = program.join('')
puts "code: " + result
puts "XML : " + result.gsub(/</, '&lt;')
print "C   : "
p result
