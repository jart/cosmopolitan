#ifndef COSMOPOLITAN_LIBC_INTERNAL_MISSIONCRITICAL_H_
#define COSMOPOLITAN_LIBC_INTERNAL_MISSIONCRITICAL_H_
#include "libc/bits/bits.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/ntdll.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/teb.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @fileoverview Mission critical system calls.
 */

#define PRINT(STR) __print(STR, tinystrlen(STR))

#define NT_HAVE_IMPORT(SLOT) \
  ((void *)*SLOT && *SLOT != (void *)&missingno /* see libc/crt/crt.S */)

#define _EXIT(rc)                                 \
  ({                                              \
    int ExitAx;                                   \
    asm volatile("syscall"                        \
                 : "=a"(ExitAx)                   \
                 : "0"(__NR_exit), "D"(rc)        \
                 : "rcx", "r11", "cc", "memory"); \
    ExitAx;                                       \
  })

#define GETPID()                                                        \
  ({                                                                    \
    int Res;                                                            \
    asm("syscall" : "=a"(Res) : "0"(__NR_getpid) : "rcx", "r11", "cc"); \
    Res;                                                                \
  })

#define KILL(pid, sig)                                \
  ({                                                  \
    int KillAx;                                       \
    unsigned char Cf;                                 \
    asm volatile(CFLAG_ASM("clc\n\t"                  \
                           "syscall")                 \
                 : CFLAG_CONSTRAINT(Cf), "=a"(KillAx) \
                 : "1"(__NR_kill), "D"(pid), "S"(sig) \
                 : "rcx", "r11", "cc", "memory");     \
    Cf ? -KillAx : KillAx;                            \
  })

#define RAISE(SIG)                                                   \
  ({                                                                 \
    int RaiseAx = -1;                                                \
    int Sig = (SIG);                                                 \
    if (Sig == SIGTRAP) {                                            \
      DebugBreak();                                                  \
    } else if (!IsWindows()) {                                       \
      RaiseAx = KILL(GETPID(), Sig);                                 \
    } else {                                                         \
      switch (Sig) {                                                 \
        case SIGINT:                                                 \
          GenerateConsoleCtrlEvent(kNtCtrlCEvent, 0);                \
          break;                                                     \
        case SIGHUP:                                                 \
          GenerateConsoleCtrlEvent(kNtCtrlCloseEvent, 0);            \
          break;                                                     \
        case SIGQUIT:                                                \
          GenerateConsoleCtrlEvent(kNtCtrlBreakEvent, 0);            \
          break;                                                     \
        default:                                                     \
          for (;;) TerminateProcess(GetCurrentProcess(), 128 + Sig); \
      }                                                              \
    }                                                                \
    RaiseAx;                                                         \
  })

#define SCHED_YIELD()                               \
  ({                                                \
    int64_t SyAx;                                   \
    if (!IsWindows()) {                             \
      asm volatile("syscall"                        \
                   : "=a"(SyAx)                     \
                   : "0"(__NR_sched_yield)          \
                   : "rcx", "r11", "cc", "memory"); \
    } else {                                        \
      NtYieldExecution();                           \
    }                                               \
    0;                                              \
  })

#define WAIT4(PID, OPT_OUT_WSTATUS, OPTIONS, OPT_OUT_RUSAGE)          \
  ({                                                                  \
    int64_t WaAx;                                                     \
    if (!IsWindows()) {                                               \
      register void *Reg10 asm("r10") = (OPT_OUT_RUSAGE);             \
      asm volatile("syscall"                                          \
                   : "=a"(WaAx)                                       \
                   : "0"(__NR_wait4), "D"(PID), "S"(OPT_OUT_WSTATUS), \
                     "d"(OPTIONS), "r"(Reg10)                         \
                   : "rcx", "r11", "cc", "memory");                   \
    } else {                                                          \
      WaAx = wait4$nt(PID, OPT_OUT_WSTATUS, OPTIONS, OPT_OUT_RUSAGE); \
    }                                                                 \
    WaAx;                                                             \
  })

#if 0
/**
 * Exits on Windows the hard way.
 */
#endif
#define NT_TERMINATE_PROCESS()                                                \
  do                                                                          \
    if (NtGetVersion() < kNtVersionFuture) {                                  \
      int64_t ax, cx;                                                         \
      do                                                                      \
        asm volatile(                                                         \
            "syscall" /* hook THIS system call */                             \
            : "=a"(ax), "=c"(cx)                                              \
            : "0"(NtGetVersion() < kNtVersionWindows8                         \
                      ? 0x0029                                                \
                      : NtGetVersion() < kNtVersionWindows81                  \
                            ? 0x002a                                          \
                            : NtGetVersion() < kNtVersionWindows10 ? 0x002b   \
                                                                   : 0x002c), \
              "1"(-1L /* GetCurrentProcess() */), "d"(42)                     \
            : "r11", "cc", "memory");                                         \
      while (!ax);                                                            \
    }                                                                         \
  while (0)

interruptfn void __print(const void *, size_t);

#define LOAD_DEFAULT_RBX() /* disabled for now b/c clang */
#define RESTORE_RBX()      /* disabled for now b/c clang */

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTERNAL_MISSIONCRITICAL_H_ */
