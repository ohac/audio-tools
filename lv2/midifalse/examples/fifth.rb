#!/usr/bin/env ruby
S_OP_0 = "0"
S_OP_1 = "1"
S_OP_2 = "2"
S_OP_3 = "3"
S_OP_4 = "4"
S_OP_5 = "5"
S_OP_6 = "6"
S_OP_7 = "7"
S_OP_8 = "8"
S_OP_9 = "9"
S_OP_10 = "a"
S_OP_11 = "b"
S_OP_12 = "c"
S_OP_13 = "d"
S_OP_14 = "e"
S_OP_15 = "f"
S_OP_16 = "x"
S_OP_NOP = " = "
S_OP_NOP2 = "\n"
S_OP_ADD = "+"
S_OP_SUB = "-"
S_OP_MUL = "*"
S_OP_DIV = "/"
S_OP_NEGATE = "_"
S_OP_AND = "&"
S_OP_OR = "|"
S_OP_NOT = "~"
S_OP_LESSTHAN = "<"
S_OP_GREATERTHAN = ">"
S_OP_EQUAL = "="
S_OP_DUP = "$"
S_OP_TUCK = "T"
S_OP_2DUP = "D"
S_OP_3DUP = "C"
S_OP_DROP = "%"
S_OP_SWAP = "\\"
S_OP_ROT = "@"
S_OP_PICK = "O"
S_OP_JMP = "J"
S_OP_JZ = "Z"
S_OP_JNZ = "N"
S_OP_RETURN = "R"

program = [
  # initial stack: status note velocity
  S_OP_TUCK, # duplicate note to top of stack
  # if note < 121
  S_OP_16, S_OP_8, S_OP_MUL, S_OP_7, S_OP_SUB, S_OP_LESSTHAN, S_OP_3, S_OP_JNZ,
    S_OP_DROP, S_OP_RETURN, # thru
  # else
    S_OP_DROP, S_OP_3DUP, S_OP_SWAP, S_OP_7, S_OP_ADD, S_OP_SWAP, # add 5th
    S_OP_RETURN
]

result = program.join('')
puts "code: " + result
puts "XML : " + result.gsub(/</, '&lt;')
print "C   : "
p result
