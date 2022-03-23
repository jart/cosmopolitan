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
#include "libc/intrin/kprintf.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/process.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

const struct AuxiliaryValue {
  const char *fmt;
  long *id;
  const char *name;
  const char *description;
} kAuxiliaryValues[] = {
    {"%-14p", &AT_EXECFD, "AT_EXECFD", "file descriptor of program"},
    {"%-14p", &AT_PHDR, "AT_PHDR", "address of elf program headers"},
    {"%-14p", &AT_PHENT, "AT_PHENT", "size of program header entry"},
    {"%-14p", &AT_PHNUM, "AT_PHNUM", "number of program headers"},
    {"%-14p", &AT_PAGESZ, "AT_PAGESZ", "system page size"},
    {"%-14p", &AT_BASE, "AT_BASE", "base address of the program interpreter"},
    {"%-14p", &AT_ENTRY, "AT_ENTRY", "entry address of executable"},
    {"%-14p", &AT_NOTELF, "AT_NOTELF", "set if not an elf"},
    {"%-14d", &AT_UID, "AT_UID", "real user id of thread"},
    {"%-14d", &AT_EUID, "AT_EUID", "effective user id of thread"},
    {"%-14d", &AT_GID, "AT_GID", "real group id of thread"},
    {"%-14d", &AT_EGID, "AT_EGID", "effective group id of thread"},
    {"%-14d", &AT_CLKTCK, "AT_CLKTCK", "frequency of times() counts"},
    {"%-14d", &AT_OSRELDATE, "AT_OSRELDATE",
     "freebsd release number, e.g. 1200086"},
    {"%-14p", &AT_PLATFORM, "AT_PLATFORM",
     "string identifying hardware platform"},
    {"%-14p", &AT_DCACHEBSIZE, "AT_DCACHEBSIZE", "data cache block size"},
    {"%-14p", &AT_ICACHEBSIZE, "AT_ICACHEBSIZE",
     "instruction cache block size"},
    {"%-14p", &AT_UCACHEBSIZE, "AT_UCACHEBSIZE", "unified cache block size"},
    {"%-14p", &AT_SECURE, "AT_SECURE",
     "for set{u,g}id binz & security blankets"},
    {"%-14s", &AT_BASE_PLATFORM, "AT_BASE_PLATFORM",
     "string identifying real platform"},
    {"%-14p", &AT_RANDOM, "AT_RANDOM", "address of sixteen random bytes"},
    {"%-14s (%p)", &AT_EXECFN, "AT_EXECFN", "pathname used to execute program"},
    {"%-14p", &AT_SYSINFO_EHDR, "AT_SYSINFO_EHDR",
     "linux virtual dso page address"},
    {"%-14p", &AT_FLAGS, "AT_FLAGS", "unused?"},
    {"%-14p", &AT_HWCAP, "AT_HWCAP", "cpu stuff"},
    {"%-14p", &AT_HWCAP2, "AT_HWCAP2", "more cpu stuff"},
    {"%-14p", &AT_STACKBASE, "AT_STACKBASE", "NetBSD stack base"},
    {"%-14p", &AT_CANARY, "AT_CANARY", "FreeBSD AT_CANARY"},
    {"%-14p", &AT_CANARYLEN, "AT_CANARYLEN", "FreeBSD AT_CANARYLEN"},
    {"%-14ld", &AT_NCPUS, "AT_NCPUS", "FreeBSD AT_NCPUS"},
    {"%-14p", &AT_PAGESIZES, "AT_PAGESIZES", "FreeBSD AT_PAGESIZES"},
    {"%-14d", &AT_PAGESIZESLEN, "AT_PAGESIZESLEN", "FreeBSD AT_PAGESIZESLEN"},
    {"%-14p", &AT_TIMEKEEP, "AT_TIMEKEEP", "FreeBSD AT_TIMEKEEP"},
    {"%-14p", &AT_STACKPROT, "AT_STACKPROT", "FreeBSD AT_STACKPROT"},
    {"%-14p", &AT_EHDRFLAGS, "AT_EHDRFLAGS", "FreeBSD AT_EHDRFLAGS"},
};

const struct AuxiliaryValue *DescribeAuxv(unsigned long x) {
  int i;
  for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
    if (*kAuxiliaryValues[i].id && x == *kAuxiliaryValues[i].id) {
      return kAuxiliaryValues + i;
    }
  }
  return NULL;
}

int main(int argc, char *argv[], char **envp) {
  long key;
  char **env;
  unsigned i;
  unsigned long *auxp;
  struct AuxiliaryValue *auxinfo;
  uint32_t varlen;
  char16_t var[PATH_MAX];
  kprintf("%nArguments:%n");
  for (i = 0; i < __argc; ++i) {
    kprintf(" ☼ %s%n", argv[i]);
  }
  kprintf("%nEnvironment:%n");
  for (env = envp; *env; ++env) {
    kprintf(" ☼ %s%n", *env);
  }
  kprintf("%nAuxiliary Values:%n");
  for (auxp = __auxv; *auxp; auxp += 2) {
    if ((auxinfo = DescribeAuxv(auxp[0]))) {
      kprintf(" ☼ %16s[%4ld] = ", auxinfo->name, auxp[0]);
      kprintf(auxinfo->fmt, auxp[1], auxp[1]);
      kprintf("  # %s%n", auxinfo->description);
    } else {
      kprintf(" ☼ %16s[%4ld] = %014p%n", "unknown", auxp[0], auxp[1]);
    }
  }
  kprintf("%nSpecial Parameters:%n");
  kprintf(" ☼ kTmpPath = %#s%n", kTmpPath);
  kprintf(" ☼ kNtSystemDirectory = %#s%n", kNtSystemDirectory);
  kprintf(" ☼ kNtWindowsDirectory = %#s%n", kNtWindowsDirectory);
  kprintf(" ☼ program_executable_name = %#s (%p)%n", GetProgramExecutableName(),
          GetProgramExecutableName());
  kprintf(" ☼ GetInterpreterExecutableName() → %#s%n",
          GetInterpreterExecutableName(_gc(malloc(1024)), 1024));
  kprintf(" ☼ RSP                  → %p%n", __builtin_frame_address(0));
  kprintf(" ☼ GetStackAddr()       → %p%n", GetStackAddr(0));
  kprintf(" ☼ GetStaticStackAddr() → %p%n", GetStaticStackAddr(0));
  kprintf(" ☼ GetStackSize()       → %p%n", GetStackSize());
  return 0;
}
