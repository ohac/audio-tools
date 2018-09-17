#!/usr/bin/env ruby
require "./common.rb"

# nanoPAD2 layout
#  37 39 41 43 45 47 49 51
#  36 38 40 42 44 46 48 50

#  36 39 41 43 45 47 49 51 swap top/bottom of left side
#  37 38 40 42 44 46 48 50

prog = [
  # initial stack: status note velocity
  OP_SWAP, OP_DUP, # stack: st vel note note
  OP_4, OP_ADD_20, # 36 (nanoPAD2 left bottom)
  # stack: st vel note note 36

  OP_SUB, OP_7, OP_JNZ, # if note != 36 then jump to else
    OP_DROP, OP_DROP,
    OP_5, OP_ADD_20, # 37
    OP_SWAP, OP_RETURN,

  # else
  # stack: st vel note 1
  OP_DROP, OP_DUP,
  OP_5, OP_ADD_20, # 37 (nanoPAD2 left top)
  # stack: st vel note note 37

  OP_SUB, OP_7, OP_JNZ, # if note != 37 then jump to else
    OP_DROP, OP_DROP,
    OP_4, OP_ADD_20, # 36
    OP_SWAP, OP_RETURN,

  # else
  # stack: st vel note 1
  OP_DROP, OP_SWAP, #thru
]

print_result(prog)
