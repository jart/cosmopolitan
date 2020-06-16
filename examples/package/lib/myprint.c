#include "examples/package/lib/myprint.h"
#include "libc/stdio/stdio.h"

void MyPrint(const char *s) {
  MyAsm(s);
}

void MyPrint2(const char *s) {
  fputs(s, stdout);
}
