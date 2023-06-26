// clang-format off
#ifdef _WIN32
#include "third_party/mold/elf/lto-win32.cc"
#else
#include "third_party/mold/elf/lto-unix.cc"
#endif
