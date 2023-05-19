#include "libc/calls/calls.h"
#include "libc/str/str.h"

// hello world with minimal build system dependencies

static ssize_t Write(int fd, const char *s) {
  return write(fd, s, strlen(s));
}

int main(int argc, char *argv[]) {
  Write(1, "hello\n");
}
