#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/str/str.h"

// hello world with minimal build system dependencies

static ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

int main(int argc, char *argv[]) {
  wchar_t B1[8];
  wchar_t B2[8];
  B1[1] = L'\0';
  B2[1] = L'\0';
  Write(1, "hello world\n");
  kprintf("%x\n", wcscmp(memcpy(B1, "\xff\xff\xff\x7f", 4),
                         memcpy(B2, "\x00\x00\x00\x80", 4)));
}
