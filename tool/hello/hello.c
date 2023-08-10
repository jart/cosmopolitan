/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

#define _HOSTLINUX   1
#define _HOSTWINDOWS 4
#define _HOSTXNU     8
#define _HOSTOPENBSD 16
#define _HOSTFREEBSD 32
#define _HOSTNETBSD  64

#ifndef SUPPORT_VECTOR
#define SUPPORT_VECTOR -1
#endif

#ifdef __aarch64__
#define IsAarch64() 1
#else
#define IsAarch64() 0
#endif

#define SupportsLinux()   (SUPPORT_VECTOR & _HOSTLINUX)
#define SupportsXnu()     (SUPPORT_VECTOR & _HOSTXNU)
#define SupportsWindows() (SUPPORT_VECTOR & _HOSTWINDOWS)
#define SupportsFreebsd() (SUPPORT_VECTOR & _HOSTFREEBSD)
#define SupportsOpenbsd() (SUPPORT_VECTOR & _HOSTOPENBSD)
#define SupportsNetbsd()  (SUPPORT_VECTOR & _HOSTNETBSD)

#define IsLinux()   (SupportsLinux() && __crt.os == _HOSTLINUX)
#define IsXnu()     (SupportsXnu() && __crt.os == _HOSTXNU)
#define IsWindows() (SupportsWindows() && __crt.os == _HOSTWINDOWS)
#define IsFreebsd() (SupportsFreebsd() && __crt.os == _HOSTFREEBSD)
#define IsOpenbsd() (SupportsOpenbsd() && __crt.os == _HOSTOPENBSD)
#define IsNetbsd()  (SupportsNetbsd() && __crt.os == _HOSTNETBSD)

#define O_RDONLY           0
#define PROT_NONE          0
#define PROT_READ          1
#define PROT_WRITE         2
#define PROT_EXEC          4
#define MAP_SHARED         1
#define MAP_PRIVATE        2
#define MAP_FIXED          16
#define MAP_ANONYMOUS      32
#define MAP_EXECUTABLE     4096
#define MAP_NORESERVE      16384
#define ELFCLASS32         1
#define ELFDATA2LSB        1
#define EM_NEXGEN32E       62
#define EM_AARCH64         183
#define ET_EXEC            2
#define ET_DYN             3
#define PT_LOAD            1
#define PT_DYNAMIC         2
#define PT_INTERP          3
#define EI_CLASS           4
#define EI_DATA            5
#define PF_X               1
#define PF_W               2
#define PF_R               4
#define AT_PHDR            3
#define AT_PHENT           4
#define AT_PHNUM           5
#define AT_PAGESZ          6
#define AT_EXECFN_LINUX    31
#define AT_EXECFN_NETBSD   2014
#define X_OK               1
#define XCR0_SSE           2
#define XCR0_AVX           4
#define PR_SET_MM          35
#define PR_SET_MM_EXE_FILE 13

#define EIO   5
#define EBADF 9

#define kNtInvalidHandleValue -1L
#define kNtStdInputHandle     -10u
#define kNtStdOutputHandle    -11u
#define kNtStdErrorHandle     -12u

#define kNtFileTypeUnknown 0x0000
#define kNtFileTypeDisk    0x0001
#define kNtFileTypeChar    0x0002
#define kNtFileTypePipe    0x0003
#define kNtFileTypeRemote  0x8000

#define kNtGenericRead  0x80000000u
#define kNtGenericWrite 0x40000000u

#define kNtFileShareRead   0x00000001u
#define kNtFileShareWrite  0x00000002u
#define kNtFileShareDelete 0x00000004u

#define kNtCreateNew        1
#define kNtCreateAlways     2
#define kNtOpenExisting     3
#define kNtOpenAlways       4
#define kNtTruncateExisting 5

#define kNtFileFlagOverlapped             0x40000000u
#define kNtFileAttributeNotContentIndexed 0x00002000u
#define kNtFileFlagBackupSemantics        0x02000000u
#define kNtFileFlagOpenReparsePoint       0x00200000u
#define kNtFileAttributeCompressed        0x00000800u
#define kNtFileAttributeTemporary         0x00000100u
#define kNtFileAttributeDirectory         0x00000010u

struct NtOverlapped {
  unsigned long Internal;
  unsigned long InternalHigh;
  union {
    struct {
      unsigned Offset;
      unsigned OffsetHigh;
    };
    void *Pointer;
  };
  long hEvent;
};

#define __dll_import(DLL, RET, FUNC, ARGS)                \
  extern RET(*__attribute__((__ms_abi__, __weak__)) FUNC) \
      ARGS __asm__("dll$" DLL ".dll$" #FUNC)

__dll_import("kernel32", void, ExitProcess, (unsigned));
__dll_import("kernel32", int, CloseHandle, (long));
__dll_import("kernel32", long, GetStdHandle, (unsigned));
__dll_import("kernel32", int, ReadFile,
             (long, void *, unsigned, unsigned *, struct NtOverlapped *));
__dll_import("kernel32", int, WriteFile,
             (long, const void *, unsigned, unsigned *, struct NtOverlapped *));

struct WinCrt {
  long fd2handle[3];
};

struct Crt {
  int os;
  int argc;
  char **argv;
  char **envp;
  long *auxv;
  int pagesz;
  int gransz;
  struct WinCrt *wincrt;
} __crt;

long SystemCall(long, long, long, long, long, long, long, int);
long CallSystem(long a, long b, long c, long d, long e, long f, long g, int x) {
  if (IsXnu() && !IsAarch64()) x |= 0x2000000;
  return SystemCall(a, b, c, d, e, f, g, x);
}

wontreturn void _Exit(int rc) {
  int numba;
  if (!IsWindows()) {
    if (IsLinux()) {
      if (IsAarch64()) {
        numba = 94;
      } else {
        numba = 60;
      }
    } else {
      numba = 1;
    }
    CallSystem(rc, 0, 0, 0, 0, 0, 0, numba);
  } else {
    ExitProcess((unsigned)rc << 8);
  }
  __builtin_unreachable();
}

static int ConvertFdToWin32Handle(int fd, long *out_handle) {
  if (fd < 0 || fd > 2) return -EBADF;
  *out_handle = __crt.wincrt->fd2handle[fd];
  return 0;
}

int sys_close(int fd) {
  if (!IsWindows()) {
    int numba;
    if (IsLinux()) {
      if (IsAarch64()) {
        numba = 57;
      } else {
        numba = 3;
      }
    } else {
      numba = 6;
    }
    return CallSystem(fd, 0, 0, 0, 0, 0, 0, numba);
  } else {
    int rc;
    long handle;
    if (!(rc = ConvertFdToWin32Handle(fd, &handle))) {
      CloseHandle(handle);
      return 0;
    } else {
      return rc;
    }
  }
}

ssize_t sys_write(int fd, const void *data, size_t size) {
  if (!IsWindows()) {
    int numba;
    if (IsLinux()) {
      if (IsAarch64()) {
        numba = 64;
      } else {
        numba = 1;
      }
    } else {
      numba = 4;
    }
    return CallSystem(fd, (long)data, size, 0, 0, 0, 0, numba);
  } else {
    ssize_t rc;
    long handle;
    uint32_t got;
    if (!(rc = ConvertFdToWin32Handle(fd, &handle))) {
      if (WriteFile(handle, data, size, &got, 0)) {
        return got;
      } else {
        return -EIO;
      }
    } else {
      return rc;
    }
  }
}

ssize_t sys_pread(int fd, void *data, size_t size, long off) {
  int numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 0x043;
    } else {
      numba = 0x011;
    }
  } else if (IsXnu()) {
    numba = 0x099;
  } else if (IsFreebsd()) {
    numba = 0x1db;
  } else if (IsOpenbsd()) {
    numba = 0x0a9;
  } else if (IsNetbsd()) {
    numba = 0x0ad;
  } else {
    __builtin_unreachable();
  }
  return SystemCall(fd, (long)data, size, off, off, 0, 0, numba);
}

int sys_access(const char *path, int mode) {
  if (IsLinux() && IsAarch64()) {
    return SystemCall(-100, (long)path, mode, 0, 0, 0, 0, 48);
  } else {
    return CallSystem((long)path, mode, 0, 0, 0, 0, 0, IsLinux() ? 21 : 33);
  }
}

int sys_open(const char *path, int flags, int mode) {
  if (IsLinux() && IsAarch64()) {
    return SystemCall(-100, (long)path, flags, mode, 0, 0, 0, 56);
  } else {
    return CallSystem((long)path, flags, mode, 0, 0, 0, 0, IsLinux() ? 2 : 5);
  }
}

int sys_mprotect(void *addr, size_t size, int prot) {
  int numba;
  // all unix operating systems define the same values for prot
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 226;
    } else {
      numba = 10;
    }
  } else {
    numba = 74;
  }
  return CallSystem((long)addr, size, prot, 0, 0, 0, 0, numba);
}

long sys_mmap(void *addr, size_t size, int prot, int flags, int fd, long off) {
  long numba;
  if (IsLinux()) {
    if (IsAarch64()) {
      numba = 222;
    } else {
      numba = 9;
    }
  } else {
    // this flag isn't supported on non-Linux systems. since it's just
    // hinting the kernel, it should be inconsequential to just ignore
    flags &= ~MAP_NORESERVE;
    // this flag is ignored by Linux and it overlaps with bsd map_anon
    flags &= ~MAP_EXECUTABLE;
    if (flags & MAP_ANONYMOUS) {
      // all bsd-style operating systems share the same mag_anon magic
      flags &= ~MAP_ANONYMOUS;
      flags |= 4096;
    }
    if (IsFreebsd()) {
      numba = 477;
    } else if (IsOpenbsd()) {
      numba = 49;
    } else {
      numba = 197;  // xnu, netbsd
    }
  }
  return CallSystem((long)addr, size, prot, flags, fd, off, off, numba);
}

wontreturn void __unix_start(long di, long *sp, char os) {

  // detect freebsd
  if (SupportsXnu() && os == _HOSTXNU) {
    os = _HOSTXNU;
  } else if (SupportsFreebsd() && di) {
    os = _HOSTFREEBSD;
    sp = (long *)di;
  }

  // extract arguments
  __crt.argc = *sp;
  __crt.argv = (char **)(sp + 1);
  __crt.envp = (char **)(sp + 1 + __crt.argc + 1);
  __crt.auxv = (long *)(sp + 1 + __crt.argc + 1);
  for (;;) {
    if (!*__crt.auxv++) {
      break;
    }
  }

  // detect openbsd
  if (SupportsOpenbsd() && !os && !__crt.auxv[0]) {
    os = _HOSTOPENBSD;
  }

  // detect netbsd and find end of words
  __crt.pagesz = IsAarch64() ? 16384 : 4096;
  for (long *ap = __crt.auxv; ap[0]; ap += 2) {
    if (ap[0] == AT_PAGESZ) {
      __crt.pagesz = ap[1];
    } else if (SupportsNetbsd() && !os && ap[0] == AT_EXECFN_NETBSD) {
      os = _HOSTNETBSD;
    }
  }
  if (!os) {
    os = _HOSTLINUX;
  }
  __crt.gransz = __crt.pagesz;
  __crt.os = os;

  // call startup routines
  typedef int init_f(int, char **, char **, long *);
  extern init_f *__init_array_start[] __attribute__((__weak__));
  extern init_f *__init_array_end[] __attribute__((__weak__));
  for (init_f **fp = __init_array_end; fp-- > __init_array_start;) {
    (*fp)(__crt.argc, __crt.argv, __crt.envp, __crt.auxv);
  }

  // call program
  int main(int, char **, char **);
  _Exit(main(__crt.argc, __crt.argv, __crt.envp));
}

wontreturn void __win32_start(void) {
  long sp[] = {
      0,  // argc
      0,  // empty argv
      0,  // empty environ
      0,  // empty auxv
  };
  __crt.wincrt = &(struct WinCrt){
      .fd2handle =
          {
              GetStdHandle(kNtStdInputHandle),
              GetStdHandle(kNtStdOutputHandle),
              GetStdHandle(kNtStdErrorHandle),
          },
  };
  __unix_start(0, sp, _HOSTWINDOWS);
}

ssize_t print(int fd, const char *s, ...) {
  int c;
  unsigned n;
  va_list va;
  char b[512];
  va_start(va, s);
  for (n = 0; s; s = va_arg(va, const char *)) {
    while ((c = *s++)) {
      if (n < sizeof(b)) {
        b[n++] = c;
      }
    }
  }
  va_end(va);
  return sys_write(fd, b, n);
}

////////////////////////////////////////////////////////////////////////////////

char data[10] = "sup";
char bss[0xf9];
_Thread_local char tdata[10] = "hello";
_Thread_local char tbss[0xf9];

_Section(".love") int main(int argc, char **argv, char **envp) {
  if (argc == 666) {
    bss[0] = data[0];
    tbss[0] = tdata[0];
  }
  print(1, "hello world\n", NULL);
}
