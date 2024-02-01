/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/utsname.h"
#include "libc/calls/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/promises.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/cpuid4.internal.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nexgen32e/x86info.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/startf.h"
#include "libc/nt/runtime.h"
#include "libc/nt/startupinfo.h"
#include "libc/nt/struct/ldrdatatableentry.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/cap.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/x86idnames.h"

__static_yoink("strerror");   // for kprintf()
__static_yoink("strsignal");  // for kprintf()

#define PRINT(FMT, ...)               \
  do {                                \
    kprintf(prologue);                \
    kprintf(FMT "\n", ##__VA_ARGS__); \
  } while (0)

static const char *FindNameById(const struct IdName *names, unsigned long id) {
  for (; names->name; names++) {
    if (names->id == id) {
      return names->name;
    }
  }
  return NULL;
}

static void PrintDependencies(const char *prologue) {
#ifdef __x86_64__
  struct NtLinkedList *head = &NtGetPeb()->Ldr->InLoadOrderModuleList;
  struct NtLinkedList *ldr = head->Next;
  do {
    const struct NtLdrDataTableEntry *dll =
        (const struct NtLdrDataTableEntry *)ldr;
    PRINT(" ☼ %.*!hs (%'zukb @ %p)", dll->FullDllName.Length,
          dll->FullDllName.Data, dll->SizeOfImage / 1024, dll->DllBase);
  } while ((ldr = ldr->Next) && ldr != head);
#endif
}

static void Print(const char *prologue) {
}

static const char *ConvertCcToStr(int cc) {
  if (cc == _POSIX_VDISABLE) {
    return "_POSIX_VDISABLE";
  } else {
    static char buf[8] = "CTRL-";
    buf[5] = CTRL(cc);
    return buf;
  }
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

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  union {
    char path[PATH_MAX];
    struct pollfd pfds[128];
  } u;
  CheckLargeStackAllocation(&u, sizeof(u));
#pragma GCC pop_options

  const struct AuxiliaryValue {
    const char *fmt;
    unsigned long id;
    const char *name;
  } kAuxiliaryValues[] = {
      {"%-14p", AT_EXECFD, "AT_EXECFD"},
      {"%-14p", AT_PHDR, "AT_PHDR"},
      {"%-14p", AT_PHENT, "AT_PHENT"},
      {"%-14p", AT_PHNUM, "AT_PHNUM"},
      {"%-14p", AT_PAGESZ, "AT_PAGESZ"},
      {"%-14p", AT_BASE, "AT_BASE"},
      {"%-14p", AT_ENTRY, "AT_ENTRY"},
      {"%-14p", AT_NOTELF, "AT_NOTELF"},
      {"%-14d", AT_UID, "AT_UID"},
      {"%-14d", AT_EUID, "AT_EUID"},
      {"%-14d", AT_GID, "AT_GID"},
      {"%-14d", AT_EGID, "AT_EGID"},
      {"%-14d", AT_CLKTCK, "AT_CLKTCK"},
      {"%-14d", AT_OSRELDATE, "AT_OSRELDATE"},
      {"%-14p", AT_PLATFORM, "AT_PLATFORM"},
      {"%-14p", AT_DCACHEBSIZE, "AT_DCACHEBSIZE"},
      {"%-14p", AT_ICACHEBSIZE, "AT_ICACHEBSIZE"},
      {"%-14p", AT_UCACHEBSIZE, "AT_UCACHEBSIZE"},
      {"%-14p", AT_SECURE, "AT_SECURE"},
      {"%-14s", AT_BASE_PLATFORM, "AT_BASE_PLATFORM"},
      {"%-14p", AT_RANDOM, "AT_RANDOM"},
      {"%-14s", AT_EXECFN, "AT_EXECFN"},
      {"%-14p", AT_SYSINFO_EHDR, "AT_SYSINFO_EHDR"},
      {"%-14p", AT_FLAGS, "AT_FLAGS"},
      {"%-14p", AT_HWCAP, "AT_HWCAP"},
      {"%-14p", AT_HWCAP2, "AT_HWCAP2"},
      {"%-14p", AT_STACKBASE, "AT_STACKBASE"},
      {"%-14p", AT_CANARY, "AT_CANARY"},
      {"%-14p", AT_CANARYLEN, "AT_CANARYLEN"},
      {"%-14ld", AT_NCPUS, "AT_NCPUS"},
      {"%-14p", AT_PAGESIZES, "AT_PAGESIZES"},
      {"%-14d", AT_PAGESIZESLEN, "AT_PAGESIZESLEN"},
      {"%-14p", AT_TIMEKEEP, "AT_TIMEKEEP"},
      {"%-14p", AT_STACKPROT, "AT_STACKPROT"},
      {"%-14p", AT_EHDRFLAGS, "AT_EHDRFLAGS"},
      {"%-14d", AT_MINSIGSTKSZ, "AT_MINSIGSTKSZ"},
  };

  int e, x;
  char **env;
  sigset_t ss;
  bool gotsome;
  unsigned i, n;
  uintptr_t *auxp;
  struct rlimit rlim;
  struct utsname uts;
  struct sigaction sa;
  struct sched_param sp;
  struct termios termios;
  const struct AuxiliaryValue *auxinfo;

  (void)x;

  if (!PLEDGED(STDIO)) return;

  ftrace_enabled(-1);
  strace_enabled(-1);
  e = errno;

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
    kprintf("\n");
  } else {
    PRINT("  uname() failed %m");
  }

  PRINT("");
  PRINT("MICROPROCESSOR");
  kprintf(prologue);
#ifdef __x86_64__
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
  kprintf("\n");
  if ((x = KCPUIDS(16H, EAX) & 0x7fff)) {
    kprintf(prologue);
    kprintf("  %dmhz %s", x, "freq");
    if ((x = KCPUIDS(16H, EBX) & 0x7fff)) {
      kprintf(" / %dmhz %s", x, "turbo");
    }
    if ((x = KCPUIDS(16H, ECX) & 0x7fff)) {
      kprintf(" / %dmhz %s", x, "bus");
    }
    kprintf("\n");
  }
  if (X86_HAVE(HYPERVISOR)) {
    unsigned eax, ebx, ecx, edx;
    asm("push\t%%rbx\n\t"
        "cpuid\n\t"
        "mov\t%%ebx,%1\n\t"
        "pop\t%%rbx"
        : "=a"(eax), "=rm"(ebx), "=c"(ecx), "=d"(edx)
        : "0"(0x40000000), "2"(0L));
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
#elif defined(__aarch64__)
  kprintf("  AARCH64\n");
#else
  kprintf("\n");
#endif

  PRINT("");
  PRINT("FILE DESCRIPTORS");
  for (i = 0; i < ARRAYLEN(u.pfds); ++i) {
    u.pfds[i].fd = i;
    u.pfds[i].events = POLLIN;
  }
  if ((n = poll(u.pfds, ARRAYLEN(u.pfds), 0)) != -1) {
    for (i = 0; i < ARRAYLEN(u.pfds); ++i) {
      char oflagbuf[128];
      if (i && (u.pfds[i].revents & POLLNVAL)) continue;
      PRINT(" ☼ %d (revents=%#hx fcntl(F_GETFL)=%s isatty()=%hhhd)", i,
            u.pfds[i].revents, (DescribeOpenFlags)(oflagbuf, fcntl(i, F_GETFL)),
            isatty(i));
    }
  } else {
    PRINT("  poll() returned %d %m", n);
  }

  if (!sigprocmask(SIG_BLOCK, 0, &ss)) {
    PRINT("");
    PRINT("SIGNAL MASK %#lx", ss);
    if (ss) {
      for (i = 1; i <= NSIG; ++i) {
        if (ss & (1ull << (i - 1))) {
          PRINT(" ☼ %G (%d) is masked", i, i);
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
  PRINT("SIGNALS");
  for (gotsome = 0, i = 1; i <= 64; ++i) {
    if (!sigaction(i, 0, &sa)) {
      if (sa.sa_handler == SIG_IGN) {
        PRINT(" ☼ %G is SIG_IGN", i);
        gotsome = 1;
      } else if (sa.sa_handler != SIG_DFL) {
        PRINT(" ☼ %G is %p", i, sa.sa_handler);
        gotsome = 1;
      }
    }
  }
  if (!gotsome) {
    PRINT(" ☼ SIG_DFL");
  }

  if (PLEDGED(PROC)) {
    PRINT("");
    PRINT("SCHEDULER");
    errno = 0;
    PRINT(" ☼ getpriority(PRIO_PROCESS) → %d% m", getpriority(PRIO_PROCESS, 0));
    errno = 0;
    PRINT(" ☼ getpriority(PRIO_PGRP)    → %d% m", getpriority(PRIO_PGRP, 0));
    errno = 0;
    PRINT(" ☼ getpriority(PRIO_USER)    → %d% m", getpriority(PRIO_USER, 0));
    errno = 0;
    PRINT(" ☼ sched_getscheduler()      → %s% m",
          DescribeSchedPolicy(sched_getscheduler(0)));
    errno = 0;
    if (sched_getparam(0, &sp) != -1) {
      PRINT(" ☼ sched_getparam()          → %d% m", sp.sched_priority);
    }
  }

  PRINT("");
  PRINT("RESOURCE LIMITS");
  for (gotsome = false, i = 0; i < RLIM_NLIMITS; ++i) {
    if (!getrlimit(i, &rlim)) {
      char buf[20];
      if (rlim.rlim_cur == RLIM_INFINITY) rlim.rlim_cur = -1;
      if (rlim.rlim_max == RLIM_INFINITY) rlim.rlim_max = -1;
      PRINT(" ☼ %-20s %,16ld %,16ld", (DescribeRlimitName)(buf, i),
            rlim.rlim_cur, rlim.rlim_max);
      gotsome = true;
    }
  }
  if (!gotsome) {
    PRINT(" ☼ %s", "none");
  }

  PRINT("");
  PRINT("STACK");
  size_t foss_stack_size = 4ul * 1024 * 1024;
  PRINT(" ☼ %p __oldstack top", ROUNDUP(__oldstack + 1, foss_stack_size));
  PRINT(" ☼ %p __oldstack ptr", __oldstack);
  PRINT(" ☼ %p __oldstack bot", ROUNDDOWN(__oldstack, foss_stack_size));
  PRINT(" ☼ %p __builtin_frame_address(0)", __builtin_frame_address(0));
  PRINT(" ☼ %p GetStackPointer()", GetStackPointer());

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
        auxinfo = 0;
        for (i = 0; i < ARRAYLEN(kAuxiliaryValues); ++i) {
          if (kAuxiliaryValues[i].id && auxp[0] == kAuxiliaryValues[i].id) {
            auxinfo = kAuxiliaryValues + i;
            break;
          }
        }
        if (auxinfo) {
          ksnprintf(u.path, sizeof(u.path), auxinfo->fmt, auxp[1]);
          PRINT(" ☼ %16s[%4ld] = %s", auxinfo->name, auxp[0], u.path);
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
  PRINT(" ☼ %s = %d", "CLK_TCK", CLK_TCK);
  PRINT(" ☼ %s = %#s", "__get_tmpdir()", __get_tmpdir());
#ifdef __x86_64__
  PRINT(" ☼ %s = %#s", "kNtSystemDirectory", kNtSystemDirectory);
  PRINT(" ☼ %s = %#s", "kNtWindowsDirectory", kNtWindowsDirectory);
#endif
  PRINT(" ☼ %s = %#s", "__argv[0]", __argv[0]);
  PRINT(" ☼ %s = %#s", "program_invocation_name", program_invocation_name);
  PRINT(" ☼ %s = %#s", "program_invocation_short_name",
        program_invocation_short_name);
  PRINT(" ☼ %s = %#s", "getenv(\"_\")", getenv("_"));
  PRINT(" ☼ %s = %#s", "getauxval(AT_EXECFN)", getauxval(AT_EXECFN));
  PRINT(" ☼ %s = %#s", "GetProgramExecutableName", GetProgramExecutableName());
  PRINT(" ☼ %s = %#s", "GetInterpreterExecutableName",
        GetInterpreterExecutableName(u.path, sizeof(u.path)));
  PRINT(" ☼ %s = %p", "GetStackSize()", GetStackSize());
  PRINT(" ☼ %s = %p", "GetGuardSize()", GetGuardSize());
  PRINT(" ☼ %s = %p", "GetStackAddr()", GetStackAddr());
  PRINT(" ☼ %s = %p", "GetStaticStackSize()", GetStaticStackSize());
  PRINT(" ☼ %s = %p", "GetStaticStackAddr(0)", GetStaticStackAddr(0));
  PRINT(" ☼ %s = %p", "__builtin_frame_address(0)", __builtin_frame_address(0));

  PRINT("");
  PRINT("MEMTRACK");
  __print_maps();

  PRINT("");
  PRINT("TERMIOS");
  for (i = 0; i <= 2; ++i) {
    if (!tcgetattr(i, &termios)) {
      struct winsize ws;
      if (i == 0) {
        PRINT("  - stdin");
      } else if (i == 1) {
        PRINT("  - stdout");
      } else {
        PRINT("  - stderr");
      }
      errno = 0;
      PRINT("    isatty = %d% m", isatty(i));
      if (!tcgetwinsize(i, &ws)) {
        PRINT("    ws_row = %d", ws.ws_row);
        PRINT("    ws_col = %d", ws.ws_col);
      } else {
        PRINT("    tcgetwinsize = %s", strerror(errno));
      }
      kprintf(prologue);
      kprintf("    c_iflag =");
      if (termios.c_iflag & IGNBRK) kprintf(" IGNBRK");
      if (termios.c_iflag & BRKINT) kprintf(" BRKINT");
      if (termios.c_iflag & IGNPAR) kprintf(" IGNPAR");
      if (termios.c_iflag & PARMRK) kprintf(" PARMRK");
      if (termios.c_iflag & INPCK) kprintf(" INPCK");
      if (termios.c_iflag & ISTRIP) kprintf(" ISTRIP");
      if (termios.c_iflag & INLCR) kprintf(" INLCR");
      if (termios.c_iflag & IGNCR) kprintf(" IGNCR");
      if (termios.c_iflag & ICRNL) kprintf(" ICRNL");
      if (termios.c_iflag & IXON) kprintf(" IXON");
      if (termios.c_iflag & IXANY) kprintf(" IXANY");
      if (termios.c_iflag & IXOFF) kprintf(" IXOFF");
      if (termios.c_iflag & IMAXBEL) kprintf(" IMAXBEL");
      if (termios.c_iflag & IUTF8) kprintf(" IUTF8");
      if (termios.c_iflag & IUCLC) kprintf(" IUCLC");
      kprintf("\n");
      kprintf(prologue);
      kprintf("    c_oflag =");
      if (termios.c_oflag & OPOST) kprintf(" OPOST");
      if (termios.c_oflag & ONLCR) kprintf(" ONLCR");
      if (termios.c_oflag & OCRNL) kprintf(" OCRNL");
      if (termios.c_oflag & ONOCR) kprintf(" ONOCR");
      if (termios.c_oflag & ONLRET) kprintf(" ONLRET");
      if (termios.c_oflag & OFILL) kprintf(" OFILL");
      if (termios.c_oflag & OFDEL) kprintf(" OFDEL");
      if (termios.c_oflag & OLCUC) kprintf(" OLCUC");
      if ((termios.c_oflag & NLDLY) == NL1) {
        kprintf(" NL1");
      } else if ((termios.c_oflag & NLDLY) == NL2) {
        kprintf(" NL2");
      } else if ((termios.c_oflag & NLDLY) == NL3) {
        kprintf(" NL3");
      }
      if ((termios.c_oflag & CRDLY) == CR1) {
        kprintf(" CR1");
      } else if ((termios.c_oflag & CRDLY) == CR2) {
        kprintf(" CR2");
      } else if ((termios.c_oflag & CRDLY) == CR3) {
        kprintf(" CR3");
      }
      if ((termios.c_oflag & TABDLY) == TAB1) {
        kprintf(" TAB1");
      } else if ((termios.c_oflag & TABDLY) == TAB2) {
        kprintf(" TAB2");
      } else if ((termios.c_oflag & TABDLY) == TAB3) {
        kprintf(" TAB3");
      }
      if ((termios.c_oflag & BSDLY) == BS1) {
        kprintf(" BS1");
      }
      if ((termios.c_oflag & VTDLY) == VT1) {
        kprintf(" VT1");
      }
      if ((termios.c_oflag & FFDLY) == FF1) {
        kprintf(" FF1");
      }
      kprintf("\n");
      kprintf(prologue);
      kprintf("    c_cflag =");
      if (termios.c_cflag & PARENB) kprintf(" PARENB");
      if (termios.c_cflag & PARODD) kprintf(" PARODD");
      if (termios.c_cflag & CSTOPB) kprintf(" CSTOPB");
      if (termios.c_cflag & PARODD) kprintf(" PARODD");
      if (termios.c_cflag & HUPCL) kprintf(" HUPCL");
      if (termios.c_cflag & CREAD) kprintf(" CREAD");
      if (termios.c_cflag & CLOCAL) kprintf(" CLOCAL");
      if ((termios.c_cflag & CSIZE) == CS5) {
        kprintf(" CS5");
      } else if ((termios.c_cflag & CSIZE) == CS6) {
        kprintf(" CS6");
      } else if ((termios.c_cflag & CSIZE) == CS7) {
        kprintf(" CS7");
      } else if ((termios.c_cflag & CSIZE) == CS8) {
        kprintf(" CS8");
      }
      kprintf("\n");
      kprintf(prologue);
      kprintf("    c_lflag =");
      if (termios.c_lflag & ISIG) kprintf(" ISIG");
      if (termios.c_lflag & ICANON) kprintf(" ICANON");
      if (termios.c_lflag & ECHO) kprintf(" ECHO");
      if (termios.c_lflag & ECHOE) kprintf(" ECHOE");
      if (termios.c_lflag & ECHOK) kprintf(" ECHOK");
      if (termios.c_lflag & ECHONL) kprintf(" ECHONL");
      if (termios.c_lflag & NOFLSH) kprintf(" NOFLSH");
      if (termios.c_lflag & TOSTOP) kprintf(" TOSTOP");
      if (termios.c_lflag & IEXTEN) kprintf(" IEXTEN");
      if (termios.c_lflag & ECHOCTL) kprintf(" ECHOCTL");
      if (termios.c_lflag & ECHOPRT) kprintf(" ECHOPRT");
      if (termios.c_lflag & ECHOKE) kprintf(" ECHOKE");
      if (termios.c_lflag & FLUSHO) kprintf(" FLUSHO");
      if (termios.c_lflag & PENDIN) kprintf(" PENDIN");
      if (termios.c_lflag & XCASE) kprintf(" XCASE");
      kprintf("\n");
      PRINT("    cfgetispeed()  = %u", cfgetispeed(&termios));
      PRINT("    cfgetospeed()  = %u", cfgetospeed(&termios));
      PRINT("    c_cc[VMIN]     = %d", termios.c_cc[VMIN]);
      PRINT("    c_cc[VTIME]    = %d", termios.c_cc[VTIME]);
      PRINT("    c_cc[VINTR]    = %s", ConvertCcToStr(termios.c_cc[VINTR]));
      PRINT("    c_cc[VQUIT]    = %s", ConvertCcToStr(termios.c_cc[VQUIT]));
      PRINT("    c_cc[VERASE]   = %s", ConvertCcToStr(termios.c_cc[VERASE]));
      PRINT("    c_cc[VKILL]    = %s", ConvertCcToStr(termios.c_cc[VKILL]));
      PRINT("    c_cc[VEOF]     = %s", ConvertCcToStr(termios.c_cc[VEOF]));
      PRINT("    c_cc[VSTART]   = %s", ConvertCcToStr(termios.c_cc[VSTART]));
      PRINT("    c_cc[VSTOP]    = %s", ConvertCcToStr(termios.c_cc[VSTOP]));
      PRINT("    c_cc[VSUSP]    = %s", ConvertCcToStr(termios.c_cc[VSUSP]));
      PRINT("    c_cc[VSWTC]    = %s", ConvertCcToStr(termios.c_cc[VSWTC]));
      PRINT("    c_cc[VREPRINT] = %s", ConvertCcToStr(termios.c_cc[VREPRINT]));
      PRINT("    c_cc[VDISCARD] = %s", ConvertCcToStr(termios.c_cc[VDISCARD]));
      PRINT("    c_cc[VWERASE]  = %s", ConvertCcToStr(termios.c_cc[VWERASE]));
      PRINT("    c_cc[VLNEXT]   = %s", ConvertCcToStr(termios.c_cc[VLNEXT]));
      PRINT("    c_cc[VEOL]     = %s", ConvertCcToStr(termios.c_cc[VEOL]));
      PRINT("    c_cc[VEOL2]    = %s", ConvertCcToStr(termios.c_cc[VEOL2]));
    } else {
      PRINT("  - tcgetattr(%d) failed %m", i);
    }
  }

  if (IsWindows()) {
    struct NtStartupInfo startinfo;
    GetStartupInfo(&startinfo);

    PRINT("");
    uint32_t cm;
    PRINT("STANDARD HANDLES");
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdInputHandle)",
          GetStdHandle(kNtStdInputHandle));
    if (GetConsoleMode(GetStdHandle(kNtStdInputHandle), &cm))
      PRINT("   %s", DescribeNtConsoleInFlags(cm));
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdOutputHandle)",
          GetStdHandle(kNtStdOutputHandle));
    if (GetConsoleMode(GetStdHandle(kNtStdOutputHandle), &cm))
      PRINT("   %s", DescribeNtConsoleOutFlags(cm));
    PRINT(" ☼ %s = %ld", "GetStdHandle(kNtStdErrorHandle)",
          GetStdHandle(kNtStdErrorHandle));
    if (GetConsoleMode(GetStdHandle(kNtStdErrorHandle), &cm))
      PRINT("   %s", DescribeNtConsoleOutFlags(cm));

#ifdef __x86_64__
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
#endif

    PRINT("");
    PRINT("DEPENDENCIES");
    PrintDependencies(prologue);
  }

  PRINT("");
  strace_enabled(+1);
  ftrace_enabled(+1);
  errno = e;
}
