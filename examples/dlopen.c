#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/calls.h"
#include "libc/dlopen/dlfcn.h"
#include "libc/fmt/itoa.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"

/**
 * @fileoverview cosmopolitan dynamic runtime linking demo
 *
 * Our cosmo_dlopen() interface currently supports:
 *
 * - x86-64 Linux w/ Glibc
 * - x86-64 Linux w/ Musl Libc
 * - x86-64 FreeBSD
 * - x86-64 Windows
 * - x86-64 NetBSD
 * - aarch64 Linux w/ Glibc
 * - aarch64 Linux w/ Musl Libc
 * - aarch64 MacOS
 *
 */

int main(int argc, char **argv, char **envp) {

  // open the host system's zlib library
  void *libc = cosmo_dlopen("libz.so", RTLD_LAZY);
  if (!libc) {
    tinyprint(2, cosmo_dlerror(), "\n", NULL);
    exit(1);
  }

  // load crc() function address
  unsigned (*crc32)(unsigned, void *, int) = cosmo_dlsym(libc, "crc32");
  if (!crc32) {
    tinyprint(2, cosmo_dlerror(), "\n", NULL);
    exit(1);
  }

  // compute a checksum and print the result
  char ibuf[12];
  FormatInt32(ibuf, crc32(0, "hi", 2));
  tinyprint(1, "crc(hi) = ", ibuf, "\n", NULL);

  // mop up
  cosmo_dlclose(libc);
  exit(0);
}
