/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/vk.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/inputrecord.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;

static const struct {
  int vk;
  int normal_str;
  int shift_str;
  int ctrl_str;
  int shift_ctrl_str;
} kVirtualKey[] = {
#define S(s) W(s "\0\0")
#define W(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])
    {kNtVkUp, S("A"), S("1;2A"), S("1;5A"), S("1;6A")},
    {kNtVkDown, S("B"), S("1;2B"), S("1;5B"), S("1;6B")},
    {kNtVkLeft, S("D"), S("1;2D"), S("1;5D"), S("1;6D")},
    {kNtVkRight, S("C"), S("1;2C"), S("1;5C"), S("1;6C")},
    {kNtVkInsert, S("2~"), S("2;2~"), S("2;5~"), S("2;6~")},
    {kNtVkDelete, S("3~"), S("3;2~"), S("3;5~"), S("3;6~")},
    {kNtVkHome, S("H"), S("1;2H"), S("1;5H"), S("1;6H")},
    {kNtVkEnd, S("F"), S("1;2F"), S("1;5F"), S("1;6F")},
    {kNtVkPrior, S("5~"), S("5;2~"), S("5;5~"), S("5;6~")},
    {kNtVkNext, S("6~"), S("6;2~"), S("6;5~"), S("6;6~")},
    {kNtVkF1, -S("OP"), S("1;2P"), S("11^"), S("1;6P")},
    {kNtVkF2, -S("OQ"), S("1;2Q"), S("12^"), S("1;6Q")},
    {kNtVkF3, -S("OR"), S("1;2R"), S("13^"), S("1;6R")},
    {kNtVkF4, -S("OS"), S("1;2S"), S("14^"), S("1;6S")},
    {kNtVkF5, S("15~"), S("28~"), S("15^"), S("28^")},
    {kNtVkF6, S("17~"), S("29~"), S("17^"), S("29^")},
    {kNtVkF7, S("18~"), S("31~"), S("18^"), S("31^")},
    {kNtVkF8, S("19~"), S("32~"), S("19^"), S("32^")},
    {kNtVkF9, S("20~"), S("33~"), S("20^"), S("33^")},
    {kNtVkF10, S("21~"), S("34~"), S("21^"), S("34^")},
    {kNtVkF11, S("23~"), S("23$"), S("23^"), S("23@")},
    {kNtVkF12, S("24~"), S("24$"), S("24^"), S("24@")},
#undef W
#undef S
};

#define KEYSTROKE_CONTAINER(e) DLL_CONTAINER(struct Keystroke, elem, e)

struct Keystroke {
  char buf[32];
  unsigned buflen;
  struct Dll elem;
};

struct Keystrokes {
  struct Dll *list;
  struct Dll *free;
  bool end_of_file;
  uint16_t utf16hs;
  unsigned allocated;
  pthread_mutex_t lock;
  struct Keystroke pool[32];
};

static struct Keystrokes __keystroke;

static textwindows void LockKeystrokes(void) {
  pthread_mutex_lock(&__keystroke.lock);
}

static textwindows void UnlockKeystrokes(void) {
  pthread_mutex_unlock(&__keystroke.lock);
}

static textwindows uint64_t BlockSignals(void) {
  return atomic_exchange(&__get_tls()->tib_sigmask, -1);
}

static textwindows void UnblockSignals(uint64_t mask) {
  atomic_store_explicit(&__get_tls()->tib_sigmask, mask, memory_order_release);
}

static textwindows int GetVirtualKey(uint16_t vk, bool shift, bool ctrl) {
  for (int i = 0; i < ARRAYLEN(kVirtualKey); ++i) {
    if (kVirtualKey[i].vk == vk) {
      if (shift && ctrl) {
        return kVirtualKey[i].shift_ctrl_str;
      } else if (shift) {
        return kVirtualKey[i].shift_str;
      } else if (ctrl) {
        return kVirtualKey[i].ctrl_str;
      } else {
        return kVirtualKey[i].normal_str;
      }
    }
  }
  return 0;
}

static textwindows int ProcessKeyEvent(const struct NtInputRecord *r, char *p) {

  uint16_t c = r->Event.KeyEvent.uChar.UnicodeChar;
  uint16_t vk = r->Event.KeyEvent.wVirtualKeyCode;
  uint16_t cks = r->Event.KeyEvent.dwControlKeyState;

  // ignore keyup events
  if (!r->Event.KeyEvent.bKeyDown && (!c || vk != kNtVkMenu)) {
    return 0;
  }

  // process virtual keys
  int n = 0;
  if (!c) {
    int w;
    w = GetVirtualKey(vk, !!(cks & kNtShiftPressed),
                      !!(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)));
    if (!w) return 0;
    p[n++] = 033;
    if (cks & (kNtLeftAltPressed | kNtRightAltPressed)) {
      p[n++] = 033;
    }
    if (w > 0) {
      p[n++] = '[';
    } else {
      w = -w;
    }
    do p[n++] = w;
    while ((w >>= 8));
    return n;
  }

  // translate utf-16 into utf-32
  if (IsHighSurrogate(c)) {
    __keystroke.utf16hs = c;
    return 0;
  }
  if (IsLowSurrogate(c)) {
    c = MergeUtf16(__keystroke.utf16hs, c);
  }

  // enter sends \r in a raw terminals
  // make it a multics newline instead
  if (c == '\r' && !(__ttymagic & kFdTtyNoCr2Nl)) {
    c = '\n';
  }

  // microsoft doesn't encode ctrl-space (^@) as nul
  // detecting it is also impossible w/ kNtEnableVirtualTerminalInput
  if (c == ' ' && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    c = '\0';
  }

  // make it possible to distinguish ctrl-h (^H) from backspace (^?)
  if (c == kNtVkBack) {
    c = 0177;
  }

  // handle ctrl-c and ctrl-\, which tcsetattr() is able to remap
  if (!(__ttymagic & kFdTtyNoIsigs)) {
    if (c == __vintr && __vintr != _POSIX_VDISABLE) {
      STRACE("encountered CTRL(%#c) c_cc[VINTR] will raise SIGINT", CTRL(c));
      __get_tls()->tib_sigpending |= 1ull << (SIGINT - 1);
      return 0;
    } else if (c == __vquit && __vquit != _POSIX_VDISABLE) {
      STRACE("encountered CTRL(%#c) c_cc[VQUITR] will raise SIGQUIT", CTRL(c));
      __get_tls()->tib_sigpending |= 1ull << (SIGQUIT - 1);
      return 0;
    }
  }

  // handle ctrl-d the end of file keystroke
  if (!(__ttymagic & kFdTtyUncanon)) {
    if (c == __veof && __veof != _POSIX_VDISABLE) {
      STRACE("encountered CTRL(%#c) c_cc[VEOF] closing console input", CTRL(c));
      __keystroke.end_of_file = true;
      return 0;
    }
  }

  // insert esc prefix when alt is held
  if ((cks & (kNtLeftAltPressed | kNtRightAltPressed)) &&
      !(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)) &&
      r->Event.KeyEvent.bKeyDown) {
    p[n++] = 033;
  }

  // convert utf-32 to utf-8
  uint64_t w = tpenc(c);
  do p[n++] = w;
  while ((w >>= 8));
  return n;
}

// To use the tty mouse events feature:
//   - write(1, "\e[?1000;1002;1015;1006h") to enable
//   - write(1, "\e[?1000;1002;1015;1006l") to disable
// See o//examples/ttyinfo.com and o//tool/viz/life.com
static textwindows int ProcessMouseEvent(const struct NtInputRecord *r,
                                         char *b) {
  int e = 0;
  char *p = b;
  uint32_t currentbs = __mousebuttons;
  uint32_t ev = r->Event.MouseEvent.dwEventFlags;
  uint32_t bs = r->Event.MouseEvent.dwButtonState;
  ev &= kNtMouseMoved | kNtMouseWheeled;
  bs &= kNtFromLeft1stButtonPressed | kNtRightmostButtonPressed;
  if (ev & kNtMouseWheeled) {
    // scroll wheel (unnatural mode)
    if (!(r->Event.MouseEvent.dwControlKeyState &
          (kNtShiftPressed | kNtLeftCtrlPressed | kNtRightCtrlPressed |
           kNtLeftAltPressed | kNtRightAltPressed))) {
      bool isup = ((int)r->Event.MouseEvent.dwButtonState >> 16) > 0;
      if (__ttymagic & kFdTtyXtMouse) {
        e = isup ? 80 : 81;
        goto OutputXtermMouseEvent;
      } else {
        // we disable mouse highlighting when the tty is put in raw mode
        // to mouse wheel events with widely understood vt100 arrow keys
        *p++ = 033;
        *p++ = '[';
        if (isup) {
          *p++ = 'A';  // \e[A up
        } else {
          *p++ = 'B';  // \e[B down
        }
      }
    }
  } else if ((bs || currentbs) && (__ttymagic & kFdTtyXtMouse)) {
    if (bs && (ev & kNtMouseMoved) && currentbs) {
      e |= 32;  // dragging
    }
    if ((bs | currentbs) & kNtRightmostButtonPressed) {
      e |= 2;  // right
    }
  OutputXtermMouseEvent:
    *p++ = 033;
    *p++ = '[';
    *p++ = '<';
    p = FormatInt32(p, e);
    *p++ = ';';
    p = FormatInt32(p, r->Event.MouseEvent.dwMousePosition.X + 1);
    *p++ = ';';
    p = FormatInt32(p, r->Event.MouseEvent.dwMousePosition.Y + 1);
    if (!bs && currentbs) {
      *p++ = 'm';  // up
    } else {
      *p++ = 'M';  // down
    }
    __mousebuttons = bs;
  }
  return p - b;
}

static textwindows int ConvertConsoleInputToAnsi(const struct NtInputRecord *r,
                                                 char p[hasatleast 32]) {
  switch (r->EventType) {
    case kNtKeyEvent:
      return ProcessKeyEvent(r, p);
    case kNtMouseEvent:
      return ProcessMouseEvent(r, p);
    case kNtWindowBufferSizeEvent:
      STRACE("detected console resize will raise SIGWINCH");
      __get_tls()->tib_sigpending |= 1ull << (SIGWINCH - 1);
      return 0;
    default:
      return 0;
  }
}

static textwindows struct Keystroke *NewKeystroke(void) {
  struct Dll *e;
  struct Keystroke *k = 0;
  int i, n = ARRAYLEN(__keystroke.pool);
  if (atomic_load_explicit(&__keystroke.allocated, memory_order_acquire) < n &&
      (i = atomic_fetch_add(&__keystroke.allocated, 1)) < n) {
    k = __keystroke.pool + i;
  } else {
    if ((e = dll_first(__keystroke.free))) {
      k = KEYSTROKE_CONTAINER(e);
      dll_remove(&__keystroke.free, &k->elem);
    }
    if (!k) {
      if (_weaken(malloc)) {
        k = _weaken(malloc)(sizeof(struct Keystroke));
      } else {
        enomem();
        return 0;
      }
    }
  }
  if (k) {
    bzero(k, sizeof(*k));
    dll_init(&k->elem);
  }
  return k;
}

static textwindows void IngestConsoleInputRecord(struct NtInputRecord *r) {
  int len;
  struct Keystroke *k;
  char buf[sizeof(k->buf)];
  if ((len = ConvertConsoleInputToAnsi(r, buf))) {
    if ((k = NewKeystroke())) {
      memcpy(k->buf, buf, sizeof(k->buf));
      k->buflen = len;
      dll_make_last(&__keystroke.list, &k->elem);
    } else {
      STRACE("ran out of memory to hold keystroke %#.*s", len, buf);
    }
  }
}

static textwindows void IngestConsoleInput(int64_t handle) {
  uint32_t i, n;
  struct NtInputRecord records[16];
  if (!__keystroke.end_of_file) {
    do {
      if (GetNumberOfConsoleInputEvents(handle, &n)) {
        if (n) {
          n = MIN(ARRAYLEN(records), n);
          if (ReadConsoleInput(handle, records, n, &n)) {
            for (i = 0; i < n && !__keystroke.end_of_file; ++i) {
              IngestConsoleInputRecord(records + i);
            }
          } else {
            STRACE("ReadConsoleInput failed w/ %d", GetLastError());
            __keystroke.end_of_file = true;
            break;
          }
        }
      } else {
        STRACE("GetNumberOfConsoleInputRecords failed w/ %d", GetLastError());
        __keystroke.end_of_file = true;
        break;
      }
    } while (n == ARRAYLEN(records));
  }
}

textwindows int FlushConsoleInputBytes(int64_t handle) {
  int rc;
  uint64_t m;
  m = BlockSignals();
  LockKeystrokes();
  if (FlushConsoleInputBuffer(handle)) {
    dll_make_first(&__keystroke.free, __keystroke.list);
    __keystroke.list = 0;
    rc = 0;
  } else {
    rc = __winerr();
  }
  UnlockKeystrokes();
  UnblockSignals(m);
  return rc;
}

textwindows int CountConsoleInputBytes(int64_t handle) {
  int count = 0;
  struct Dll *e;
  uint64_t m = BlockSignals();
  LockKeystrokes();
  IngestConsoleInput(handle);
  for (e = dll_first(__keystroke.list); e; e = dll_next(__keystroke.list, e)) {
    count += KEYSTROKE_CONTAINER(e)->buflen;
  }
  if (!count && __keystroke.end_of_file) {
    count = -1;
  }
  UnlockKeystrokes();
  UnblockSignals(m);
  return count;
}

static textwindows bool DigestConsoleInput(void *data, size_t size, int *rc) {
  struct Dll *e;
  if ((e = dll_first(__keystroke.list))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    uint32_t got = MIN(size, k->buflen);
    uint32_t remain = k->buflen - got;
    if (got) memcpy(data, k->buf, got);
    if (remain) memmove(k->buf, k->buf + got, remain);
    if (!remain) {
      dll_remove(&__keystroke.list, e);
      dll_make_first(&__keystroke.free, e);
    }
    k->buflen = remain;
    if (got) {
      *rc = got;
      return true;
    }
  } else if (__keystroke.end_of_file) {
    *rc = 0;
    return true;
  }
  return false;
}

// Manual CMD.EXE echoing for when !ICANON && ECHO is the case.
static textwindows void EchoTerminalInput(struct Fd *f, char *p, size_t n) {
  int64_t hOutput;
  if (f->kind == kFdConsole) {
    hOutput = f->extra;
  } else {
    hOutput = g_fds.p[1].handle;
  }
  if (__ttymagic & kFdTtyEchoRaw) {
    WriteFile(hOutput, p, n, 0, 0);
  } else {
    size_t i;
    for (i = 0; i < n; ++i) {
      if (isascii(p[i]) && iscntrl(p[i]) && p[i] != '\n' && p[i] != '\t') {
        char ctl[2];
        ctl[0] = '^';
        ctl[1] = p[i] ^ 0100;
        WriteFile(hOutput, ctl, 2, 0, 0);
      } else {
        WriteFile(hOutput, p + i, 1, 0, 0);
      }
    }
  }
}

static textwindows ssize_t ReadFromWindowsConsole(struct Fd *f, void *data,
                                                  size_t size) {
  int rc = -1;
  for (;;) {
    bool done = false;
    uint64_t m;
    m = BlockSignals();
    LockKeystrokes();
    IngestConsoleInput(f->handle);
    done = DigestConsoleInput(data, size, &rc);
    UnlockKeystrokes();
    UnblockSignals(m);
    if (done) break;
    if (f->flags & O_NONBLOCK) return eagain();
    uint32_t ms = __SIG_POLL_INTERVAL_MS;
    if (__ttymagic & kFdTtyNoBlock) {
      if (!__vtime) {
        return 0;
      } else {
        ms = __vtime * 100;
      }
    }
    if (_check_interrupts(kSigOpRestartable)) return -1;
    if (__pause_thread(ms)) return -1;
  }
  if (rc > 0 && (__ttymagic & kFdTtyEchoing)) {
    EchoTerminalInput(f, data, size);
  }
  return rc;
}

textwindows ssize_t sys_read_nt_impl(int fd, void *data, size_t size,
                                     int64_t offset) {

  bool32 ok;
  struct Fd *f;
  uint32_t got;
  int64_t handle;
  struct PosixThread *pt;

  f = g_fds.p + fd;
  handle = f->handle;
  pt = _pthread_self();
  pt->abort_errno = EAGAIN;
  size = MIN(size, 0x7ffff000);

  bool pwriting = offset != -1;
  bool seekable = f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk;
  bool nonblock = !!(f->flags & O_NONBLOCK);

  if (pwriting && !seekable) {
    return espipe();
  }
  if (!pwriting) {
    offset = 0;
  }

  uint32_t cm;
  if (!seekable && (f->kind == kFdConsole || GetConsoleMode(handle, &cm))) {
    return ReadFromWindowsConsole(f, data, size);
  }

  if (!pwriting && seekable) {
    pthread_mutex_lock(&f->lock);
    offset = f->pointer;
  }

  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 0, 0, 0),
                                 .Pointer = offset};
  // the win32 manual says it's important to *not* put &got here
  // since for overlapped i/o, we always use GetOverlappedResult
  ok = ReadFile(handle, data, size, 0, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending) {
  BlockingOperation:
    if (!nonblock) {
      pt->ioverlap = &overlap;
      pt->iohandle = handle;
    }
    if (nonblock) {
      CancelIoEx(handle, &overlap);
    } else if (_check_interrupts(kSigOpRestartable)) {
    Interrupted:
      pt->abort_errno = errno;
      CancelIoEx(handle, &overlap);
    } else {
      for (;;) {
        uint32_t i;
        if (g_fds.stdin.inisem) {
          ReleaseSemaphore(g_fds.stdin.inisem, 1, 0);
        }
        i = WaitForSingleObject(overlap.hEvent, __SIG_IO_INTERVAL_MS);
        if (i == kNtWaitTimeout) {
          if (_check_interrupts(kSigOpRestartable)) {
            goto Interrupted;
          }
        } else {
          break;
        }
      }
    }
    pt->ioverlap = 0;
    pt->iohandle = 0;
    ok = true;
  }
  if (ok) {
    // overlapped is allocated on stack, so it's important we wait
    // for windows to acknowledge that it's done using that memory
    ok = GetOverlappedResult(handle, &overlap, &got, nonblock);
    if (!ok && GetLastError() == kNtErrorIoIncomplete) {
      goto BlockingOperation;
    }
  }
  __imp_CloseHandle(overlap.hEvent);  // __imp_ to avoid log noise

  if (!pwriting && seekable) {
    if (ok) f->pointer = offset + got;
    pthread_mutex_unlock(&f->lock);
  }

  if (ok) {
    return got;
  }

  errno_t err;
  if (_weaken(pthread_testcancel_np) &&
      (err = _weaken(pthread_testcancel_np)())) {
    return ecanceled();
  }

  switch (GetLastError()) {
    case kNtErrorBrokenPipe:    // broken pipe
    case kNtErrorNoData:        // closing named pipe
    case kNtErrorHandleEof:     // pread read past EOF
      return 0;                 //
    case kNtErrorAccessDenied:  // read doesn't return EACCESS
      return ebadf();           //
    case kNtErrorOperationAborted:
      errno = pt->abort_errno;
      return -1;
    default:
      return __winerr();
  }
}

textwindows ssize_t sys_read_nt(int fd, const struct iovec *iov, size_t iovlen,
                                int64_t opt_offset) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1) return einval();
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      // TODO(jart): disable cancelations after first iteration
      if (!iov[i].iov_len) continue;
      rc = sys_read_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset);
      if (rc == -1) {
        if (total && errno != ECANCELED) {
          return total;
        } else {
          return -1;
        }
      }
      total += rc;
      if (opt_offset != -1) opt_offset += rc;
      if (rc < iov[i].iov_len) break;
    }
    return total;
  } else {
    return sys_read_nt_impl(fd, NULL, 0, opt_offset);
  }
}

#endif /* __x86_64__ */
