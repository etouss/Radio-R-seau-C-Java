
#ifndef __debug__
#define __debug__

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#define TTY "/dev/ttys000"

#endif

void write_tty(const char * info, ...);
void breakpoint();

