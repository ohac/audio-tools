#include <stdio.h>
#include <stdint.h>

#include "script.h"

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

static char* note_onoff_handler =
  // initial stack: status note velocity
  S_OP_TUCK // duplicate note to top of stack
  // if note < 121
  S_OP_16 S_OP_8 S_OP_MUL S_OP_7 S_OP_SUB S_OP_LESSTHAN S_OP_3 S_OP_JNZ
    S_OP_DROP S_OP_RETURN // thru
  // else
    S_OP_DROP S_OP_3DUP S_OP_SWAP S_OP_7 S_OP_ADD S_OP_SWAP // add 5th
    S_OP_RETURN
  ;

int main()
{
  uint8_t script_stack[256];
  int script_stackp;

  script_stack[0] = 0;
  script_stack[1] = 9;
  script_stack[2] = 100;
  script_stackp = run_script(note_onoff_handler, script_stack, 3);

  fprintf(stderr, "%s\n", note_onoff_handler);
  fprintf(stderr, "%d\n", script_stackp);

  while (script_stackp >= 3) {
    script_stackp -= 3;
    fprintf(stderr, "%02x %02x %02x\n",
        script_stack[script_stackp],
        script_stack[script_stackp + 1],
        script_stack[script_stackp + 2]);
  }

  script_stack[0] = 0;
  script_stack[1] = 125;
  script_stack[2] = 100;
  script_stackp = run_script(note_onoff_handler, script_stack, 3);

  fprintf(stderr, "%d\n", script_stackp);

  while (script_stackp >= 3) {
    script_stackp -= 3;
    fprintf(stderr, "%02x %02x %02x\n",
        script_stack[script_stackp],
        script_stack[script_stackp + 1],
        script_stack[script_stackp + 2]);
  }
  return 0;
}
