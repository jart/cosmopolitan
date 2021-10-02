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
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nt/process.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"

const struct AuxiliaryValue {
  const char *fmt;
  long *id;
  const char *name;
  const char *description;
} kAuxiliaryValues[] = {
    {"%012lx", &AT_EXECFD, "AT_EXECFD", "file descriptor of program"},
    {"%012lx", &AT_PHDR, "AT_PHDR", "address of elf program headers"},
    {"%012lx", &AT_PHENT, "AT_PHENT", "size of program header entry"},
    {"%012lx", &AT_PHNUM, "AT_PHNUM", "number of program headers"},
    {"%012lx", &AT_PAGESZ, "AT_PAGESZ", "system page size"},
    {"%012lx", &AT_BASE, "AT_BASE", "base address of the program interpreter"},
    {"%012lx", &AT_ENTRY, "AT_ENTRY", "entry address of executable"},
    {"%012lx", &AT_NOTELF, "AT_NOTELF", "set if not an elf"},
    {"%-12d", &AT_UID, "AT_UID", "real user id of thread"},
    {"%-12d", &AT_EUID, "AT_EUID", "effective user id of thread"},
    {"%-12d", &AT_GID, "AT_GID", "real group id of thread"},
    {"%-12d", &AT_EGID, "AT_EGID", "effective group id of thread"},
    {"%-12d", &AT_CLKTCK, "AT_CLKTCK", "frequency of times() counts"},
    {"%012lx", &AT_OSRELDATE, "AT_OSRELDATE",
     "freebsd release number, e.g. 1200086"},
    {"%012lx", &AT_PLATFORM, "AT_PLATFORM",
     "string identifying hardware platform"},
    {"%012lx", &AT_DCACHEBSIZE, "AT_DCACHEBSIZE", "data cache block size"},
    {"%012lx", &AT_ICACHEBSIZE, "AT_ICACHEBSIZE",
     "instruction cache block size"},
    {"%012lx", &AT_UCACHEBSIZE, "AT_UCACHEBSIZE", "unified cache block size"},
    {"%012lx", &AT_SECURE, "AT_SECURE",
     "for set{u,g}id binz & security blankets"},
    {"%-12s", &AT_BASE_PLATFORM, "AT_BASE_PLATFORM",
     "string identifying real platform"},
    {"%012lx", &AT_RANDOM, "AT_RANDOM", "address of sixteen random bytes"},
    {"%-12s", &AT_EXECFN, "AT_EXECFN", "pathname used to execute program"},
    {"%012lx", &AT_SYSINFO_EHDR, "AT_SYSINFO_EHDR",
     "linux virtual dso page address"},
    {"%012lx", &AT_FLAGS, "AT_FLAGS", "unused?"},
    {"%012lx", &AT_HWCAP, "AT_HWCAP", "cpu stuff"},
    {"%012lx", &AT_HWCAP2, "AT_HWCAP2", "more cpu stuff"},
};

const struct AuxiliaryValue *DescribeAuxv(unsigned long x) {
  int i;
  for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
    if (x == *kAuxiliaryValues[i].id) {
      return kAuxiliaryValues + i;
    }
  }
  return NULL;
}

int main(int argc, char *argv[], char **envp) {
  long key;
  unsigned i;
  unsigned long *auxp;
  char fmt[64], **env;
  struct AuxiliaryValue *auxinfo;
  uint32_t varlen;
  char16_t var[PATH_MAX];
  printf("\nArguments:\n");
  for (i = 0; i < __argc; ++i) {
    printf(" ☼ %s\n", argv[i]);
  }
  printf("\nEnvironment:\n");
  for (env = envp; *env; ++env) {
    printf(" ☼ %s\n", *env);
  }
  printf("\nAuxiliary Values:\n");
  for (auxp = __auxv; *auxp; auxp += 2) {
    if ((auxinfo = DescribeAuxv(auxp[0]))) {
      stpcpy(stpcpy(stpcpy(fmt, "%16s[%4ld] = "), auxinfo->fmt), "  # %s\n");
      printf(fmt, auxinfo->name, auxp[0], auxp[1], auxinfo->description);
    } else {
      printf("%16s[%4ld] = %012lx\n", "unknown", auxp[0], auxp[1]);
    }
  }
  printf("\nSpecial Directories:\n");
  printf(" ☼ kTmpPath = %`'s\n", kTmpPath);
  printf(" ☼ kNtSystemDirectory = %`'s\n", kNtSystemDirectory);
  printf(" ☼ kNtWindowsDirectory = %`'s\n", kNtWindowsDirectory);
  printf(" ☼ program_executable_name = %`'s\n", program_executable_name);
  return 0;
}
