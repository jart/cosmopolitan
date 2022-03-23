/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/strace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/auxv.h"

static const struct AuxiliaryValue {
  const char *fmt;
  long *id;
  const char *name;
} kAuxiliaryValues[] = {
    {"%-14p", &AT_EXECFD, "AT_EXECFD"},
    {"%-14p", &AT_PHDR, "AT_PHDR"},
    {"%-14p", &AT_PHENT, "AT_PHENT"},
    {"%-14p", &AT_PHNUM, "AT_PHNUM"},
    {"%-14p", &AT_PAGESZ, "AT_PAGESZ"},
    {"%-14p", &AT_BASE, "AT_BASE"},
    {"%-14p", &AT_ENTRY, "AT_ENTRY"},
    {"%-14p", &AT_NOTELF, "AT_NOTELF"},
    {"%-14d", &AT_UID, "AT_UID"},
    {"%-14d", &AT_EUID, "AT_EUID"},
    {"%-14d", &AT_GID, "AT_GID"},
    {"%-14d", &AT_EGID, "AT_EGID"},
    {"%-14d", &AT_CLKTCK, "AT_CLKTCK"},
    {"%-14d", &AT_OSRELDATE, "AT_OSRELDATE"},
    {"%-14p", &AT_PLATFORM, "AT_PLATFORM"},
    {"%-14p", &AT_DCACHEBSIZE, "AT_DCACHEBSIZE"},
    {"%-14p", &AT_ICACHEBSIZE, "AT_ICACHEBSIZE"},
    {"%-14p", &AT_UCACHEBSIZE, "AT_UCACHEBSIZE"},
    {"%-14p", &AT_SECURE, "AT_SECURE"},
    {"%-14s", &AT_BASE_PLATFORM, "AT_BASE_PLATFORM"},
    {"%-14p", &AT_RANDOM, "AT_RANDOM"},
    {"%-14s", &AT_EXECFN, "AT_EXECFN"},
    {"%-14p", &AT_SYSINFO_EHDR, "AT_SYSINFO_EHDR"},
    {"%-14p", &AT_FLAGS, "AT_FLAGS"},
    {"%-14p", &AT_HWCAP, "AT_HWCAP"},
    {"%-14p", &AT_HWCAP2, "AT_HWCAP2"},
    {"%-14p", &AT_STACKBASE, "AT_STACKBASE"},
    {"%-14p", &AT_CANARY, "AT_CANARY"},
    {"%-14p", &AT_CANARYLEN, "AT_CANARYLEN"},
    {"%-14ld", &AT_NCPUS, "AT_NCPUS"},
    {"%-14p", &AT_PAGESIZES, "AT_PAGESIZES"},
    {"%-14d", &AT_PAGESIZESLEN, "AT_PAGESIZESLEN"},
    {"%-14p", &AT_TIMEKEEP, "AT_TIMEKEEP"},
    {"%-14p", &AT_STACKPROT, "AT_STACKPROT"},
    {"%-14p", &AT_EHDRFLAGS, "AT_EHDRFLAGS"},
};

static const struct AuxiliaryValue *DescribeAuxv(unsigned long x) {
  int i;
  for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
    if (*kAuxiliaryValues[i].id && x == *kAuxiliaryValues[i].id) {
      return kAuxiliaryValues + i;
    }
  }
  return NULL;
}

textstartup void __printargs(void) {
#ifdef SYSDEBUG
  long key;
  char **env;
  unsigned i;
  uintptr_t *auxp;
  char path[PATH_MAX];
  struct AuxiliaryValue *auxinfo;
  STRACE("ARGUMENTS (%p)", __argv);
  for (i = 0; i < __argc; ++i) {
    STRACE(" ☼ %s", __argv[i]);
  }
  STRACE("ENVIRONMENT (%p)", __envp);
  for (env = __envp; *env; ++env) {
    STRACE(" ☼ %s", *env);
  }
  STRACE("AUXILIARY (%p)", __auxv);
  for (auxp = __auxv; *auxp; auxp += 2) {
    if ((auxinfo = DescribeAuxv(auxp[0]))) {
      ksnprintf(path, sizeof(path), auxinfo->fmt, auxp[1]);
      STRACE(" ☼ %16s[%4ld] = %s", auxinfo->name, auxp[0], path);
    } else {
      STRACE(" ☼ %16s[%4ld] = %014p", "unknown", auxp[0], auxp[1]);
    }
  }
  STRACE("SPECIALS");
  STRACE(" ☼ %30s = %#s", "kTmpPath", kTmpPath);
  STRACE(" ☼ %30s = %#s", "kNtSystemDirectory", kNtSystemDirectory);
  STRACE(" ☼ %30s = %#s", "kNtWindowsDirectory", kNtWindowsDirectory);
  STRACE(" ☼ %30s = %#s", "program_executable_name",
         GetProgramExecutableName());
  STRACE(" ☼ %30s = %#s", "GetInterpreterExecutableName()",
         GetInterpreterExecutableName(path, sizeof(path)));
  STRACE(" ☼ %30s = %p", "RSP", __builtin_frame_address(0));
  STRACE(" ☼ %30s = %p", "GetStackAddr()", GetStackAddr(0));
  STRACE(" ☼ %30s = %p", "GetStaticStackAddr(0)", GetStaticStackAddr(0));
  STRACE(" ☼ %30s = %p", "GetStackSize()", GetStackSize());
#endif
}
