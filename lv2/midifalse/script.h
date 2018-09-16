/*
  Copyright 2018 OHASHI Hideya <ohachige@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

// This language is based on the FALSE programming language.
// http://strlen.com/false-language/

#define OP_0 '0'
#define OP_1 '1'
#define OP_2 '2'
#define OP_3 '3'
#define OP_4 '4'
#define OP_5 '5'
#define OP_6 '6'
#define OP_7 '7'
#define OP_8 '8'
#define OP_9 '9'
#define OP_10 'a'
#define OP_11 'b'
#define OP_12 'c'
#define OP_13 'd'
#define OP_14 'e'
#define OP_15 'f'
#define OP_16 'x'
#define OP_NOP ' '
#define OP_NOP2 '\n'
#define OP_ADD '+'
#define OP_SUB '-'
#define OP_MUL '*'
#define OP_DIV '/'
#define OP_NEGATE '_'
#define OP_AND '&'
#define OP_OR '|'
#define OP_NOT '~'
#define OP_LESSTHAN '<'
#define OP_GREATERTHAN '>'
#define OP_EQUAL '='
#define OP_DUP '$'
#define OP_TUCK 'T'
#define OP_2DUP 'D'
#define OP_3DUP 'C'
#define OP_DROP '%'
#define OP_SWAP '\\'
#define OP_ROT '@'
#define OP_PICK 'O'
#define OP_JMP 'J'
#define OP_JZ 'Z'
#define OP_JNZ 'N'
#define OP_RETURN 'R'

#define S_OP_0 "0"
#define S_OP_1 "1"
#define S_OP_2 "2"
#define S_OP_3 "3"
#define S_OP_4 "4"
#define S_OP_5 "5"
#define S_OP_6 "6"
#define S_OP_7 "7"
#define S_OP_8 "8"
#define S_OP_9 "9"
#define S_OP_10 "a"
#define S_OP_11 "b"
#define S_OP_12 "c"
#define S_OP_13 "d"
#define S_OP_14 "e"
#define S_OP_15 "f"
#define S_OP_16 "x"
#define S_OP_NOP " "
#define S_OP_NOP2 "\n"
#define S_OP_ADD "+"
#define S_OP_SUB "-"
#define S_OP_MUL "*"
#define S_OP_DIV "/"
#define S_OP_NEGATE "_"
#define S_OP_AND "&"
#define S_OP_OR "|"
#define S_OP_NOT "~"
#define S_OP_LESSTHAN "<"
#define S_OP_GREATERTHAN ">"
#define S_OP_EQUAL "="
#define S_OP_DUP "$"
#define S_OP_TUCK "T"
#define S_OP_2DUP "D"
#define S_OP_3DUP "C"
#define S_OP_DROP "%"
#define S_OP_SWAP "\\"
#define S_OP_ROT "@"
#define S_OP_PICK "O"
#define S_OP_JMP "J"
#define S_OP_JZ "Z"
#define S_OP_JNZ "N"
#define S_OP_RETURN "R"

static const uint8_t* note_onoff_handler = (const uint8_t *)
  // initial stack: status note velocity
  S_OP_TUCK // duplicate note to top of stack
  // if note < 121
  S_OP_16 S_OP_8 S_OP_MUL S_OP_7 S_OP_SUB S_OP_LESSTHAN S_OP_3 S_OP_JNZ
    S_OP_DROP S_OP_RETURN // thru
  // else
    S_OP_DROP S_OP_3DUP S_OP_SWAP S_OP_7 S_OP_ADD S_OP_SWAP // add 5th
    S_OP_RETURN
  ;

static int run_script(const uint8_t *script, uint8_t *stackp, int stacki)
{
  int pc = 0;
  uint8_t tmp;
  while (1) {
    uint8_t op = script[pc];
    switch (op) {
    case OP_0:
    case OP_1:
    case OP_2:
    case OP_3:
    case OP_4:
    case OP_5:
    case OP_6:
    case OP_7:
    case OP_8:
    case OP_9:
      stackp[stacki] = op - '0';
      stacki++;
      pc++;
      break;
    case OP_10:
    case OP_11:
    case OP_12:
    case OP_13:
    case OP_14:
    case OP_15:
      stackp[stacki] = op - 'a' + 10;
      stacki++;
      pc++;
      break;
    case OP_16:
      stackp[stacki] = 16;
      stacki++;
      pc++;
      break;
    case OP_DUP:
      stackp[stacki] = stackp[stacki - 1];
      stacki++;
      pc++;
      break;
    case OP_TUCK:
      stackp[stacki] = stackp[stacki - 2];
      stacki++;
      pc++;
      break;
    case OP_DROP:
      stacki--;
      pc++;
      break;
    case OP_2DUP:
      stackp[stacki] = stackp[stacki - 2];
      stacki++;
      stackp[stacki] = stackp[stacki - 2];
      stacki++;
      pc++;
      break;
    case OP_3DUP:
      stackp[stacki] = stackp[stacki - 3];
      stacki++;
      stackp[stacki] = stackp[stacki - 3];
      stacki++;
      stackp[stacki] = stackp[stacki - 3];
      stacki++;
      pc++;
      break;
    case OP_ADD:
      stacki--;
      stackp[stacki - 1] += stackp[stacki];
      pc++;
      break;
    case OP_SUB:
      stacki--;
      stackp[stacki - 1] -= stackp[stacki];
      pc++;
      break;
    case OP_MUL:
      stacki--;
      stackp[stacki - 1] *= stackp[stacki];
      pc++;
      break;
    case OP_DIV:
      stacki--;
      if (stackp[stacki] > 0) {
        stackp[stacki - 1] /= stackp[stacki];
      }
      pc++;
      break;
    case OP_LESSTHAN:
      stacki--;
      stackp[stacki - 1] = stackp[stacki - 1] < stackp[stacki] ? 1 : 0;
      pc++;
      break;
    case OP_JMP:
      stacki--;
      pc += stackp[stacki];
      break;
    case OP_JZ:
      stacki--;
      if (!stackp[stacki - 1]) {
        pc += stackp[stacki];
      } else {
        pc++;
      }
      break;
    case OP_JNZ:
      stacki--;
      if (stackp[stacki - 1]) {
        pc += stackp[stacki];
      } else {
        pc++;
      }
      break;
    case OP_SWAP:
      tmp = stackp[stacki - 2];
      stackp[stacki - 2] = stackp[stacki - 1];
      stackp[stacki - 1] = tmp;
      pc++;
      break;
    case OP_RETURN:
      return stacki;
    default:
      pc++;
      break;
    }
  }
}
