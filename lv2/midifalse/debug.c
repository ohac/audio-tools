#include <stdio.h>
#include <stdint.h>

#include "script.h"

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
