#include "debug.h"

void write_tty(const char * info,...){
  va_list arguments;
  va_start(arguments,info);
  static int tty = 0;
  if(!tty){
    tty = open(TTY,O_WRONLY|O_APPEND);
  }
  vdprintf(tty, info, arguments);
  va_end(arguments);
}
void breakpoint(){}

