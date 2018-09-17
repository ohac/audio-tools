#!/usr/bin/env ruby
require "./common.rb"

# nanoPAD2 layout
#  37 39 41 43 45 47 49 51
#  36 38 40 42 44 46 48 50
# to
#  36 50 48 46 44 42 40 38
#  37 51 49 47 45 43 41 39

layout_pre = [
  [OP_4, OP_ADD_20], # 36
  [OP_5, OP_ADD_20], # 37
  [OP_6, OP_ADD_20], # 38
  [OP_7, OP_ADD_20], # 39
  [OP_8, OP_ADD_20], # 40
  [OP_9, OP_ADD_20], # 41
  [OP_10, OP_ADD_20], # 42
  [OP_11, OP_ADD_20], # 43
  [OP_12, OP_ADD_20], # 44
  [OP_13, OP_ADD_20], # 45
  [OP_14, OP_ADD_20], # 46
  [OP_15, OP_ADD_20], # 47
  [OP_0, OP_ADD_30], # 48
  [OP_1, OP_ADD_30], # 49
  [OP_2, OP_ADD_30], # 50
  [OP_3, OP_ADD_30], # 51
].map{|f| f.join('')}

layout_post = [
  [OP_5, OP_ADD_20], # 37
  [OP_4, OP_ADD_20], # 36
  [OP_3, OP_ADD_30], # 51
  [OP_2, OP_ADD_30], # 50
  [OP_1, OP_ADD_30], # 49
  [OP_0, OP_ADD_30], # 48
  [OP_15, OP_ADD_20], # 47
  [OP_14, OP_ADD_20], # 46
  [OP_13, OP_ADD_20], # 45
  [OP_12, OP_ADD_20], # 44
  [OP_11, OP_ADD_20], # 43
  [OP_10, OP_ADD_20], # 42
  [OP_9, OP_ADD_20], # 41
  [OP_8, OP_ADD_20], # 40
  [OP_7, OP_ADD_20], # 39
  [OP_6, OP_ADD_20], # 38
].map{|f| f.join('')}

sub = layout_post.size.times.to_a.map do |i|
  [
    # initial stack: st vel note
    OP_DUP,
    layout_pre[i], # stack: st vel note note pre
    OP_SUB, OP_7, OP_JNZ, # if note != pre then jump to else
      OP_DROP, OP_DROP,
      layout_post[i],
      OP_SWAP, OP_RETURN,
    # else
    # stack: st vel note 1
    OP_DROP,
  ].join('')
end

prog = [
  # initial stack: status note velocity
  OP_SWAP, # stack: st vel note
  sub.join(''),
  OP_SWAP, #thru
]

print_result(prog)
