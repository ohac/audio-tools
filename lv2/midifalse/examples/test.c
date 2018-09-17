#include <stdio.h>
#include <stdint.h>

#include "../script.h"

static char* fifth = "Tx8*7-<3N%R%C\\7+\\R";
static char* ignore_off = "C%%x/8-3Z%R%%%%R";
static char* vel_off = "$x8*7-<5Z%7+R%%x8*1-R";
static char* ch_off = "@1+@@";

int test(char* script, uint8_t v1, uint8_t v2, uint8_t v3)
{
  uint8_t script_stack[256] = "";
  int script_stackp = 3;

  fprintf(stderr, "%s\n", script);

  script_stack[0] = v1;
  script_stack[1] = v2;
  script_stack[2] = v3;
  script_stackp = run_script(script, script_stack, script_stackp);

  fprintf(stderr, "%d\n", script_stackp);
  while (script_stackp >= 3) {
    script_stackp -= 3;
    fprintf(stderr, "%02x %02x %02x\n",
        script_stack[script_stackp],
        script_stack[script_stackp + 1],
        script_stack[script_stackp + 2]);
  }
}

int main()
{
  test(fifth, 0x90, 9, 100);
  test(fifth, 0x80, 125, 100);
  test(ignore_off, 0x80, 9, 100);
  test(ignore_off, 0x92, 9, 100);
  test(vel_off, 0x90, 11, 127);
  test(vel_off, 0x90, 11, 120);
  test(vel_off, 0x90, 11, 119);
  test(vel_off, 0x90, 11, 118);
  test(vel_off, 0x90, 11, 0);
  test(ch_off, 0x90, 12, 90);
  test(ch_off, 0x8e, 12, 90);
  return 0;
}
