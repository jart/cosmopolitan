/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/maps.h"
#include "libc/intrin/nomultics.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nt/accounting.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filemapflags.h"
#include "libc/nt/enum/pageflags.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/pedef.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/signals.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/systeminfo.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/winargs.internal.h"
#include "libc/serialize.h"
#include "libc/sock/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

#define abi __msabi textwindows dontinstrument

// clang-format off
__msabi extern typeof(AddVectoredExceptionHandler) *const __imp_AddVectoredExceptionHandler;
__msabi extern typeof(CreateFileMapping) *const __imp_CreateFileMappingW;
__msabi extern typeof(DuplicateHandle) *const __imp_DuplicateHandle;
__msabi extern typeof(FreeEnvironmentStrings) *const __imp_FreeEnvironmentStringsW;
__msabi extern typeof(GetCommandLine) *const __imp_GetCommandLineW;
__msabi extern typeof(GetConsoleMode) *const __imp_GetConsoleMode;
__msabi extern typeof(GetCurrentDirectory) *const __imp_GetCurrentDirectoryW;
__msabi extern typeof(GetCurrentProcessId) *const __imp_GetCurrentProcessId;
__msabi extern typeof(GetEnvironmentStrings) *const __imp_GetEnvironmentStringsW;
__msabi extern typeof(GetEnvironmentVariable) *const __imp_GetEnvironmentVariableW;
__msabi extern typeof(GetFileAttributes) *const __imp_GetFileAttributesW;
__msabi extern typeof(GetStdHandle) *const __imp_GetStdHandle;
__msabi extern typeof(GetSystemInfo) *const __imp_GetSystemInfo;
__msabi extern typeof(GetSystemInfo) *const __imp_GetSystemInfo;
__msabi extern typeof(GetUserName) *const __imp_GetUserNameW;
__msabi extern typeof(MapViewOfFileEx) *const __imp_MapViewOfFileEx;
__msabi extern typeof(SetConsoleCP) *const __imp_SetConsoleCP;
__msabi extern typeof(SetConsoleMode) *const __imp_SetConsoleMode;
__msabi extern typeof(SetConsoleOutputCP) *const __imp_SetConsoleOutputCP;
__msabi extern typeof(SetEnvironmentVariable) *const __imp_SetEnvironmentVariableW;
__msabi extern typeof(SetStdHandle) *const __imp_SetStdHandle;
__msabi extern typeof(VirtualProtectEx) *const __imp_VirtualProtectEx;
__msabi extern typeof(WriteFile) *const __imp_WriteFile;
// clang-format on

void cosmo(int, char **, char **, long (*)[2]) wontreturn;
void __stack_call(int, char **, char **, long (*)[2],
                  void (*)(int, char **, char **, long (*)[2]),
                  intptr_t) wontreturn;

bool __winmain_isfork;
intptr_t __winmain_jmpbuf[5];
struct CosmoTib *__winmain_tib;

__funline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

abi static char16_t *StrStr(const char16_t *haystack, const char16_t *needle) {
  size_t i;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i])
        return (/*unconst*/ char16_t *)haystack;
      if (!haystack[i])
        break;
      if (needle[i] != haystack[i])
        break;
    }
    if (!*haystack++)
      break;
  }
  return 0;
}

abi static void PrintError(const char *s, size_t n) {
#define PrintError(s) PrintError(s, sizeof(s) - 1)
  __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), s, n, 0, 0);
}

// detect the unholiest of environments
abi static bool32 IsWslChimera(void) {
  char16_t path[PATH_MAX];
  return __imp_GetCurrentDirectoryW(PATH_MAX, path) &&  //
         path[0] == '\\' &&                             //
         path[1] == '\\' &&                             //
         path[2] == 'w' &&                              //
         path[3] == 's' &&                              //
         path[4] == 'l';
}

// returns true if utf-8 path is a win32-style path that exists
abi static bool32 WinFileExists(const char *path) {
  uint16_t path16[PATH_MAX];
  size_t z = ARRAYLEN(path16);
  size_t n = tprecode8to16(path16, z, path).ax;
  if (n >= z - 1)
    return false;
  return __imp_GetFileAttributesW(path16) != -1u;
}

// this ensures close(1) won't accidentally close(2) for example
abi static void DeduplicateStdioHandles(void) {
  for (long i = 0; i < 3; ++i) {
    int64_t h1 = __imp_GetStdHandle(kNtStdio[i]);
    for (long j = i + 1; j < 3; ++j) {
      int64_t h2 = __imp_GetStdHandle(kNtStdio[j]);
      if (h1 == h2) {
        int64_t h3;
        __imp_DuplicateHandle(-1, h2, -1, &h3, 0, false,
                              kNtDuplicateSameAccess);
        __imp_SetStdHandle(kNtStdio[j], h3);
      }
    }
  }
}

abi static bool32 HasEnvironmentVariable(const char16_t *name) {
  char16_t buf[4];
  return __imp_GetEnvironmentVariableW(name, buf, ARRAYLEN(buf));
}

abi static unsigned OnWinCrash(struct NtExceptionPointers *ep) {
  int code, sig = __sig_crash_sig(ep->ExceptionRecord->ExceptionCode, &code);
  TerminateThisProcess(sig);
}

// main function of windows init process
// i.e. first process spawned that isn't forked
abi wontreturn static void WinInit(const char16_t *cmdline) {
  __oldstack = (intptr_t)__builtin_frame_address(0);

  __imp_SetConsoleOutputCP(kNtCpUtf8);

  // make console into utf-8 ansi/xterm style tty
  if ((intptr_t)v_ntsubsystem == kNtImageSubsystemWindowsCui) {
    __imp_SetConsoleCP(kNtCpUtf8);
    __imp_SetConsoleOutputCP(kNtCpUtf8);
    for (int i = 0; i <= 2; ++i) {
      uint32_t m;
      intptr_t h = __imp_GetStdHandle(kNtStdio[i]);
      if (__imp_GetConsoleMode(h, &m)) {
        if (!i) {
          m |= kNtEnableMouseInput | kNtEnableWindowInput |
               kNtEnableProcessedInput;
        } else {
          m &= ~kNtDisableNewlineAutoReturn;
          m |= kNtEnableProcessedOutput | kNtEnableVirtualTerminalProcessing;
        }
        __imp_SetConsoleMode(h, m);
      }
    }
  }

  // so crash signals can be reported to cosmopolitan bash
  __imp_AddVectoredExceptionHandler(true, (void *)OnWinCrash);

  // allocate memory for stack and argument block
  intptr_t stackhand;
  char *stackaddr = (char *)GetStaticStackAddr(0);
  size_t stacksize = GetStaticStackSize();
  __imp_MapViewOfFileEx(
      (stackhand = __imp_CreateFileMappingW(-1, 0, kNtPageExecuteReadwrite,
                                            stacksize >> 32, stacksize, NULL)),
      kNtFileMapWrite | kNtFileMapExecute, 0, 0, stacksize, stackaddr);
  int stackprot = (intptr_t)ape_stack_prot;
  if (~stackprot & PROT_EXEC) {
    uint32_t old;
    __imp_VirtualProtectEx(GetCurrentProcess(), stackaddr, stacksize,
                           kNtPageReadwrite, &old);
  }
  uint32_t oldattr;
  __imp_VirtualProtectEx(GetCurrentProcess(), stackaddr, GetGuardSize(),
                         kNtPageReadwrite | kNtPageGuard, &oldattr);
  if (_weaken(__maps_stack)) {
    struct NtSystemInfo si;
    __imp_GetSystemInfo(&si);
    _weaken(__maps_stack)(stackaddr, si.dwPageSize, GetGuardSize(), stacksize,
                          stackprot, stackhand);
  }
  struct WinArgs *wa =
      (struct WinArgs *)(stackaddr + (stacksize - sizeof(struct WinArgs)));

  // define $TERM if it's not already present
  // programs like emacs will stop the world and nag if it's not set
  if (!HasEnvironmentVariable(u"TERM")) {
    __imp_SetEnvironmentVariableW(u"TERM", u"xterm-256color");
  }

  // define $USER as GetUserName() if not set
  // Windows doesn't define this environment variable by default
  uint32_t vsize = ARRAYLEN(wa->tmp16);
  if (!HasEnvironmentVariable(u"USER") &&
      __imp_GetUserNameW(&wa->tmp16, &vsize)) {
    __imp_SetEnvironmentVariableW(u"USER", wa->tmp16);
  }

  // define $HOME as $HOMEDRIVE$HOMEPATH if not set
  // Windows doesn't define this environment variable by default
  uint32_t vlen;
  if (!HasEnvironmentVariable(u"HOME") &&
      (vlen = __imp_GetEnvironmentVariableW(u"HOMEDRIVE", wa->tmp16,
                                            ARRAYLEN(wa->tmp16))) <
          ARRAYLEN(wa->tmp16) &&
      (vlen += __imp_GetEnvironmentVariableW(u"HOMEPATH", wa->tmp16 + vlen,
                                             ARRAYLEN(wa->tmp16) - vlen)) <
          ARRAYLEN(wa->tmp16) &&
      vlen) {
    __imp_SetEnvironmentVariableW(u"HOME", wa->tmp16);
  }

  // parse utf-16 command into utf-8 argv array in argument block
  int count = GetDosArgv(cmdline, wa->argblock, ARRAYLEN(wa->argblock),
                         wa->argv, ARRAYLEN(wa->argv));

  // normalize executable path
  if (wa->argv[0] && !WinFileExists(wa->argv[0])) {
    unsigned i, n = 0;
    while (wa->argv[0][n])
      ++n;
    if (n + 4 < sizeof(wa->argv0buf)) {
      for (i = 0; i < n; ++i) {
        wa->argv0buf[i] = wa->argv[0][i];
      }
      WRITE32LE(wa->argv0buf + i, READ32LE(".com"));
      if (WinFileExists(wa->argv0buf)) {
        wa->argv[0] = wa->argv0buf;
      }
    }
  }

  // munge argv so dos paths become cosmo paths
  for (int i = 0; wa->argv[i]; ++i) {
    if (wa->argv[i][0] == '\\' &&  //
        wa->argv[i][1] == '\\') {
      // don't munge new technology style paths
      continue;
    }
    if (!WinFileExists(wa->argv[i])) {
      // don't munge if we're not certain it's a file
      continue;
    }
    // use forward slashes
    for (int j = 0; wa->argv[i][j]; ++j) {
      if (wa->argv[i][j] == '\\') {
        wa->argv[i][j] = '/';
      }
    }
    // turn c:/... into /c/...
    if (IsAlpha(wa->argv[i][0]) &&  //
        wa->argv[i][1] == ':' &&    //
        wa->argv[i][2] == '/') {
      wa->argv[i][1] = wa->argv[i][0];
      wa->argv[i][0] = '/';
    }
  }

  // translate utf-16 win32 environment into utf-8 environment variables
  char16_t *env16 = __imp_GetEnvironmentStringsW();
  GetDosEnviron(env16, wa->envblock, ARRAYLEN(wa->envblock) - 8, wa->envp,
                ARRAYLEN(wa->envp) - 1);
  __imp_FreeEnvironmentStringsW(env16);
  __envp = &wa->envp[0];
  // handover control to cosmopolitan runtime
  __stack_call(count, wa->argv, wa->envp, wa->auxv, cosmo,
               (uintptr_t)(stackaddr + (stacksize - sizeof(struct WinArgs))));
}

static int Atoi(const char16_t *str) {
  int c;
  unsigned x = 0;
  while ((c = *str++)) {
    if ('0' <= c && c <= '9') {
      x *= 10;
      x += c - '0';
    } else {
      return -1;
    }
  }
  return x;
}

abi static int WinGetPid(const char16_t *var, bool *out_is_inherited) {
  uint32_t len;
  char16_t val[12];
  if ((len = __imp_GetEnvironmentVariableW(var, val, ARRAYLEN(val)))) {
    int pid = -1;
    if (len < ARRAYLEN(val))
      pid = Atoi(val);
    __imp_SetEnvironmentVariableW(var, NULL);
    if (pid > 0) {
      *out_is_inherited = true;
      return pid;
    }
  }
  *out_is_inherited = false;
  return __imp_GetCurrentProcessId();
}

abi int64_t WinMain(int64_t hInstance, int64_t hPrevInstance,
                    const char *lpCmdLine, int64_t nCmdShow) {
  static atomic_ulong fake_process_signals;
  const char16_t *cmdline;
  extern char os asm("__hostos");
  os = _HOSTWINDOWS;  // madness https://news.ycombinator.com/item?id=21019722
  kStartTsc = rdtsc();
  __tls_enabled = false;
  ftrace_enabled(-1);
  if (!IsTiny() && IsWslChimera()) {
    PrintError("error: APE is running on WIN32 inside WSL. You need to run: "
               "sudo sh -c 'echo -1 > /proc/sys/fs/binfmt_misc/WSLInterop'\n");
    return 77 << 8;  // exit(77)
  }
  struct NtSystemInfo si;
  __imp_GetSystemInfo(&si);
  __pagesize = si.dwPageSize;
  __gransize = si.dwAllocationGranularity;
  bool pid_is_inherited;
  __pid = WinGetPid(u"_COSMO_PID", &pid_is_inherited);
  if (!(__sig.process = __sig_map_process(__pid, kNtOpenAlways)))
    __sig.process = &fake_process_signals;
  if (__winmain_isfork)
    __builtin_longjmp(__winmain_jmpbuf, 1);
  if (!pid_is_inherited)
    atomic_store_explicit(__sig.process, 0, memory_order_release);
  cmdline = __imp_GetCommandLineW();
#if SYSDEBUG
  // sloppy flag-only check for early initialization
  if (StrStr(cmdline, u"--strace"))
    ++__strace;
#endif
  ftrace_enabled(+1);
  if (_weaken(WinSockInit))
    _weaken(WinSockInit)();
  DeduplicateStdioHandles();
  WinInit(cmdline);
}

#endif /* __x86_64__ */
