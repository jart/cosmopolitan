#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/progn.internal.h"
#include "libc/log/log.h"
#include "libc/macros.h"
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
    {"%p", &AT_EXECFD, "AT_EXECFD", "file descriptor of program"},
    {"%p", &AT_PHDR, "AT_PHDR", "address of elf program headers"},
    {"%p", &AT_PHENT, "AT_PHENT", "size of program header entry"},
    {"%p", &AT_PHNUM, "AT_PHNUM", "number of program headers"},
    {"%p", &AT_PAGESZ, "AT_PAGESZ", "system page size"},
    {"%p", &AT_BASE, "AT_BASE", "base address of the program interpreter"},
    {"%p", &AT_ENTRY, "AT_ENTRY", "entry address of executable"},
    {"%p", &AT_NOTELF, "AT_NOTELF", "set if not an elf"},
    {"%-12d", &AT_UID, "AT_UID", "real user id of thread"},
    {"%-12d", &AT_EUID, "AT_EUID", "effective user id of thread"},
    {"%-12d", &AT_GID, "AT_GID", "real group id of thread"},
    {"%-12d", &AT_EGID, "AT_EGID", "effective group id of thread"},
    {"%-12d", &AT_CLKTCK, "AT_CLKTCK", "frequency of times() counts"},
    {"%p", &AT_OSRELDATE, "AT_OSRELDATE",
     "freebsd release number, e.g. 1200086"},
    {"%p", &AT_PLATFORM, "AT_PLATFORM", "string identifying hardware platform"},
    {"%p", &AT_DCACHEBSIZE, "AT_DCACHEBSIZE", "data cache block size"},
    {"%p", &AT_ICACHEBSIZE, "AT_ICACHEBSIZE", "instruction cache block size"},
    {"%p", &AT_UCACHEBSIZE, "AT_UCACHEBSIZE", "unified cache block size"},
    {"%p", &AT_SECURE, "AT_SECURE", "for set{u,g}id binz & security blankets"},
    {"%-12s", &AT_BASE_PLATFORM, "AT_BASE_PLATFORM",
     "string identifying real platform"},
    {"%p", &AT_RANDOM, "AT_RANDOM", "address of sixteen random bytes"},
    {"%-12s", &AT_EXECFN, "AT_EXECFN", "pathname used to execute program"},
    {"%p", &AT_SYSINFO_EHDR, "AT_SYSINFO_EHDR",
     "linux virtual dso page address"},
};

int main(int argc, char *argv[], char **envp) {
  long key;
  unsigned i;
  unsigned long val;
  char fmt[64], **env;
  printf("\nArguments:\n");
  for (i = 0; i < g_argc; ++i) {
    printf(" ☼ %s\n", argv[i]);
  }
  printf("\nEnvironment:\n");
  for (env = envp; *env; ++env) {
    printf(" ☼ %s\n", *env);
  }
  printf("\nAuxiliary Values:\n");
  for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
    key = *kAuxiliaryValues[i].id;
    val = getauxval(key);
    printf(PROGN(stpcpy(stpcpy(stpcpy(fmt, "%16s[%p] = "),
                               kAuxiliaryValues[i].fmt),
                        "  # %s\n"),
                 fmt),
           kAuxiliaryValues[i].name, key, val, kAuxiliaryValues[i].description);
  }
  printf("\nSpecial Directories:\n");
  printf(" ☼ kTmpPath = %`'s\n", kTmpPath);
  printf(" ☼ kNtSystemDirectory = %`'s\n", kNtSystemDirectory);
  printf(" ☼ kNtWindowsDirectory = %`'s\n", kNtWindowsDirectory);
  return 0;
}
