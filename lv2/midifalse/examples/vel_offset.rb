#!/usr/bin/env ruby
require "./common.rb"

program = [
  OP_DUP, # duplicate velocity
  OP_16, OP_8, OP_MUL, OP_7, OP_SUB, # 128 - 7
  OP_LESSTHAN, OP_5, OP_JZ, # if vel >= 121 jump to else
    OP_DROP,
    OP_7, OP_ADD, # vel += 7
    OP_RETURN,
  # else
    OP_DROP, OP_DROP,
    OP_16, OP_8, OP_MUL, OP_1, OP_SUB, # 128 - 1
]

result = program.join('')
puts "code: " + result
puts "XML : " + result.gsub(/</, '&lt;')
print "C   : "
p result
