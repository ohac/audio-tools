#!/usr/bin/env ruby
require "./common.rb"

program = [
	S_OP_DUP, # duplicate velocity
  S_OP_16, S_OP_8, S_OP_MUL, S_OP_7, S_OP_SUB, # 128 - 7
	S_OP_LESSTHAN, S_OP_5, S_OP_JZ, # if vel >= 121 jump to else
	  S_OP_DROP,
		S_OP_7, S_OP_ADD, # vel += 7
	  S_OP_RETURN,
	# else
	  S_OP_DROP, S_OP_DROP,
		S_OP_16, S_OP_8, S_OP_MUL, S_OP_1, S_OP_SUB, # 128 - 1
		S_OP_RETURN,
]

result = program.join('')
puts "code: " + result
puts "XML : " + result.gsub(/</, '&lt;')
print "C   : "
p result
