#include "libc/stdio/stdio.h"
#include "libc/runtime/runtime.h"

int main(int argc, char* argv[]) {
  fprintf(stderr, "%s (%s)\n", argv[0], GetProgramExecutableName());
  for (char **p = environ; *p; ++p) {
    printf("%s\n", *p);
  }
  return 0;
}
