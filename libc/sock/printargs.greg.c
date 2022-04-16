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
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/dns/dns.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/cpuid4.internal.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/ldrdatatableentry.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sig.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/x86idnames.h"

STATIC_YOINK("strsignal");  // for kprintf()

#define PRINT(FMT, ...)               \
  do {                                \
    kprintf(prologue);                \
    kprintf(FMT "%n", ##__VA_ARGS__); \
  } while (0)

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

static const char *FindNameById(const struct IdName *names, unsigned long id) {
  for (; names->name; names++) {
    if (names->id == id) {
      return names->name;
    }
  }
  return NULL;
}

static const struct AuxiliaryValue *DescribeAuxv(unsigned long x) {
  int i;
  for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
    if (*kAuxiliaryValues[i].id && x == *kAuxiliaryValues[i].id) {
      return kAuxiliaryValues + i;
    }
  }
  return NULL;
}

/**
 * Prints lots of information about this process, e.g.
 *
 *     __printargs("");
 *
 * This is called automatically in MODE=dbg if `--strace` is used.
 *
 * @param prologue needs to be a .rodata kprintf string
 */
textstartup void __printargs(const char *prologue) {
  long key;
  char **env;
  sigset_t ss;
  unsigned i, n;
  uintptr_t *auxp;
  struct utsname uts;
  char path[PATH_MAX];
  int x, st, ft, flags;
  struct pollfd pfds[128];
  struct AuxiliaryValue *auxinfo;
  st = __strace, __strace = 0;
  ft = g_ftrace, g_ftrace = 0;

  PRINT("");
  PRINT("SYSTEM");
  if (!uname(&uts)) {
    kprintf(prologue);
    kprintf("  %s", uts.nodename);
    if (*uts.sysname) {
      kprintf(" on %s", uts.sysname);
      if (*uts.release) {
        kprintf(" %s", uts.release);
      }
    }
    kprintf("%n");
  } else {
    PRINT("  uname() failed %m");
  }

  PRINT("");
  PRINT("MICROPROCESSOR");
  kprintf(prologue);
  kprintf("  %.*s%.*s%.*s", 4, &KCPUIDS(0H, EBX), 4, &KCPUIDS(0H, EDX), 4,
          &KCPUIDS(0H, ECX));
  if (getx86processormodel(kX86ProcessorModelKey)) {
    kprintf(" %s",
            FindNameById(kX86MarchNames,
                         getx86processormodel(kX86ProcessorModelKey)->march));
  }
  if (getx86processormodel(kX86ProcessorModelKey)) {
    kprintf(" (%s Grade)",
            FindNameById(kX86GradeNames,
                         getx86processormodel(kX86ProcessorModelKey)->grade));
  }
  kprintf("%n");
  if ((x = KCPUIDS(16H, EAX) & 0x7fff)) {
    kprintf(prologue);
    kprintf("  %dmhz %s", x, "freq");
    if ((x = KCPUIDS(16H, EBX) & 0x7fff)) {
      kprintf(" / %dmhz %s", x, "turbo");
    }
    if ((x = KCPUIDS(16H, ECX) & 0x7fff)) {
      kprintf(" / %dmhz %s", x, "bus");
    }
    kprintf("%n");
  }
  if (X86_HAVE(HYPERVISOR)) {
    unsigned eax, ebx, ecx, edx;
    asm("push\t%%rbx\n\t"
        "cpuid\n\t"
        "mov\t%%ebx,%1\n\t"
        "pop\t%%rbx"
        : "=a"(eax), "=rm"(ebx), "=c"(ecx), "=d"(edx)
        : "0"(0x40000000), "2"(0));
    PRINT("  Running inside %.4s%.4s%.4s (eax=%#x)", &ebx, &ecx, &edx, eax);
  }
  CPUID4_ITERATE(i, {
    PRINT("  L%d%s%s %u-way %,u byte cache w/%s "
          "%,u sets of %,u byte lines shared across %u threads%s",
          CPUID4_CACHE_LEVEL,
          CPUID4_CACHE_TYPE == 1   ? " data"
          : CPUID4_CACHE_TYPE == 2 ? " code"
                                   : "",
          CPUID4_IS_FULLY_ASSOCIATIVE ? " fully-associative" : "",
          CPUID4_WAYS_OF_ASSOCIATIVITY, CPUID4_CACHE_SIZE_IN_BYTES,
          CPUID4_PHYSICAL_LINE_PARTITIONS > 1 ? " physically partitioned" : "",
          CPUID4_NUMBER_OF_SETS, CPUID4_SYSTEM_COHERENCY_LINE_SIZE,
          CPUID4_MAX_THREADS_SHARING_CACHE,
          CPUID4_COMPLEX_INDEXING ? " complexly-indexed" : "");
  });
  kprintf(prologue);
  kprintf(" ");
  if (X86_HAVE(SSE3)) kprintf(" SSE3");
  if (X86_HAVE(SSSE3)) kprintf(" SSSE3");
  if (X86_HAVE(SSE4_2)) kprintf(" SSE4_2");
  if (X86_HAVE(POPCNT)) kprintf(" POPCNT");
  if (X86_HAVE(AVX)) kprintf(" AVX");
  if (X86_HAVE(AVX2)) kprintf(" AVX2");
  if (X86_HAVE(FMA)) kprintf(" FMA");
  if (X86_HAVE(BMI)) kprintf(" BMI");
  if (X86_HAVE(BMI2)) kprintf(" BMI2");
  if (X86_HAVE(ADX)) kprintf(" ADX");
  if (X86_HAVE(F16C)) kprintf(" F16C");
  if (X86_HAVE(SHA)) kprintf(" SHA");
  if (X86_HAVE(AES)) kprintf(" AES");
  if (X86_HAVE(RDRND)) kprintf(" RDRND");
  if (X86_HAVE(RDSEED)) kprintf(" RDSEED");
  if (X86_HAVE(RDTSCP)) kprintf(" RDTSCP");
  if (X86_HAVE(RDPID)) kprintf(" RDPID");
  if (X86_HAVE(LA57)) kprintf(" LA57");
  if (X86_HAVE(FSGSBASE)) kprintf(" FSGSBASE");
  kprintf("%n");

  PRINT("");
  PRINT("FILE DESCRIPTORS");
  for (i = 0; i < ARRAYLEN(pfds); ++i) {
    pfds[i].fd = i;
    pfds[i].events = POLLIN | POLLOUT;
  }
  if ((n = poll(pfds, ARRAYLEN(pfds), 20)) != -1) {
    for (i = 0; i < ARRAYLEN(pfds); ++i) {
      if (i && (pfds[i].revents & POLLNVAL)) continue;
      PRINT(" ☼ %d (revents=%#hx F_GETFL=%#x)", i, pfds[i].revents,
            fcntl(i, F_GETFL));
    }
  } else {
    PRINT("  poll() returned %d %m", n);
  }

  if (!sigprocmask(SIG_BLOCK, 0, &ss)) {
    PRINT("");
    PRINT("SIGNALS {%#lx, %#lx}", ss.__bits[0], ss.__bits[1]);
    if (ss.__bits[0] || ss.__bits[1]) {
      for (i = 0; i < 32; ++i) {
        if (ss.__bits[0] & (1u << i)) {
          PRINT(" ☼ %G (%d) is masked", i + 1, i + 1);
        }
      }
    } else {
      PRINT("  no signals blocked");
    }
  } else {
    PRINT("");
    PRINT("SIGNALS");
    PRINT("  error: sigprocmask() failed %m");
  }

  PRINT("");
  PRINT("ARGUMENTS (%p)", __argv);
  if (*__argv) {
    for (i = 0; i < __argc; ++i) {
      PRINT(" ☼ %s", __argv[i]);
    }
  } else {
    PRINT("  none");
  }

  PRINT("");
  PRINT("ENVIRONMENT (%p)", __envp);
  if (*__envp) {
    for (env = __envp; *env; ++env) {
      PRINT(" ☼ %s", *env);
    }
  } else {
    PRINT("  none");
  }

  PRINT("");
  PRINT("AUXILIARY (%p)", __auxv);
  if (*__auxv) {
    if (*__auxv) {
      for (auxp = __auxv; *auxp; auxp += 2) {
        if ((auxinfo = DescribeAuxv(auxp[0]))) {
          ksnprintf(path, sizeof(path), auxinfo->fmt, auxp[1]);
          PRINT(" ☼ %16s[%4ld] = %s", auxinfo->name, auxp[0], path);
        } else {
          PRINT(" ☼ %16s[%4ld] = %014p", "unknown", auxp[0], auxp[1]);
        }
      }
    }
  } else {
    PRINT("  none");
  }

  PRINT("");
  PRINT("SPECIALS");
  umask((i = umask(022)));
  PRINT(" ☼ %s = %#o", "umask()", i);
  PRINT(" ☼ %s = %d", "getpid()", getpid());
  PRINT(" ☼ %s = %d", "getppid()", getppid());
  PRINT(" ☼ %s = %d", "getpgrp()", getpgrp());
  PRINT(" ☼ %s = %d", "getsid()", getsid(0));
  PRINT(" ☼ %s = %d", "getuid()", getuid());
  PRINT(" ☼ %s = %d", "geteuid()", geteuid());
  PRINT(" ☼ %s = %d", "getgid()", getgid());
  PRINT(" ☼ %s = %d", "getegid()", getegid());
  PRINT(" ☼ %s = %#s", "kTmpPath", kTmpPath);
  PRINT(" ☼ %s = %#s", "kNtSystemDirectory", kNtSystemDirectory);
  PRINT(" ☼ %s = %#s", "kNtWindowsDirectory", kNtWindowsDirectory);
  PRINT(" ☼ %s = %#s", "program_executable_name", GetProgramExecutableName());
  PRINT(" ☼ %s = %#s", "GetInterpreterExecutableName()",
        GetInterpreterExecutableName(path, sizeof(path)));
  PRINT(" ☼ %s = %p", "RSP", __builtin_frame_address(0));
  PRINT(" ☼ %s = %p", "GetStackAddr()", GetStackAddr(0));
  PRINT(" ☼ %s = %p", "GetStaticStackAddr(0)", GetStaticStackAddr(0));
  PRINT(" ☼ %s = %p", "GetStackSize()", GetStackSize());

  PRINT("");
  PRINT("MEMTRACK");
  PrintMemoryIntervals(2, &_mmi);

  if (IsWindows()) {
    struct NtStartupInfo startinfo;
    GetStartupInfo(&startinfo);

    PRINT("");
    PRINT("GETSTARTUPINFO");
    if (startinfo.lpDesktop)
      PRINT(" ☼ %s = %#!hs", "lpDesktop", startinfo.lpDesktop);
    if (startinfo.lpTitle) PRINT(" ☼ %s = %#!hs", "lpTitle", startinfo.lpTitle);
    if (startinfo.dwX) PRINT(" ☼ %s = %u", "dwX", startinfo.dwX);
    if (startinfo.dwY) PRINT(" ☼ %s = %u", "dwY", startinfo.dwY);
    if (startinfo.dwXSize) PRINT(" ☼ %s = %u", "dwXSize", startinfo.dwXSize);
    if (startinfo.dwYSize) PRINT(" ☼ %s = %u", "dwYSize", startinfo.dwYSize);
    if (startinfo.dwXCountChars)
      PRINT(" ☼ %s = %u", "dwXCountChars", startinfo.dwXCountChars);
    if (startinfo.dwYCountChars)
      PRINT(" ☼ %s = %u", "dwYCountChars", startinfo.dwYCountChars);
    if (startinfo.dwFillAttribute)
      PRINT(" ☼ %s = %u", "dwFillAttribute", startinfo.dwFillAttribute);
    if (startinfo.dwFlags)
      PRINT(" ☼ %s = %s", "dwFlags", DescribeNtStartFlags(startinfo.dwFlags));
    if (startinfo.wShowWindow)
      PRINT(" ☼ %s = %hu", "wShowWindow", startinfo.wShowWindow);
    if (startinfo.cbReserved2)
      PRINT(" ☼ %s = %hu", "cbReserved2", startinfo.cbReserved2);
    if (startinfo.hStdInput)
      PRINT(" ☼ %s = %ld", "hStdInput", startinfo.hStdInput);
    if (startinfo.hStdOutput)
      PRINT(" ☼ %s = %ld", "hStdOutput", startinfo.hStdOutput);
    if (startinfo.hStdError)
      PRINT(" ☼ %s = %ld", "hStdError", startinfo.hStdError);

    PRINT("");
    PRINT("STANDARD HANDLES");
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdInputHandle)",
          GetStdHandle(kNtStdInputHandle));
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdOutputHandle)",
          GetStdHandle(kNtStdOutputHandle));
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdErrorHandle)",
          GetStdHandle(kNtStdErrorHandle));

    PRINT("");
    PRINT("TEB");
    PRINT(" ☼ gs:0x%02x %s = %p", 0x00, "NtGetSeh()", _NtGetSeh());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x08, "NtGetStackHigh()", _NtGetStackHigh());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x10, "NtGetStackLow()", _NtGetStackLow());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x18, "_NtGetSubsystemTib()",
          _NtGetSubsystemTib());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x20, "NtGetFib()", _NtGetFib());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x30, "NtGetTeb()", NtGetTeb());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x38, "NtGetEnv()", _NtGetEnv());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x40, "NtGetPid()", NtGetPid());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x48, "NtGetTid()", NtGetTid());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x50, "NtGetRpc()", _NtGetRpc());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x58, "NtGetTls()", _NtGetTls());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x60, "NtGetPeb()", NtGetPeb());
    PRINT(" ☼ gs:0x%02x %s = %p", 0x68, "NtGetErr()", NtGetErr());

    PRINT("");
    PRINT("DEPENDENCIES");
    struct NtLinkedList *head = &NtGetPeb()->Ldr->InLoadOrderModuleList;
    struct NtLinkedList *ldr = head->Next;
    do {
      const struct NtLdrDataTableEntry *dll =
          (const struct NtLdrDataTableEntry *)ldr;
      PRINT(" ☼ %.*!hs (%'zukb)", dll->FullDllName.Length,
            dll->FullDllName.Data, dll->SizeOfImage / 1024);
    } while ((ldr = ldr->Next) && ldr != head);
  }

  PRINT("");
  __strace = st;
  g_ftrace = ft;
}
