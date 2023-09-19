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
#include "libc/calls/calls.h"
#include "libc/calls/console.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/vk.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/inputrecord.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"

#ifdef __x86_64__

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;

static const struct {
  uint16_t vk;
  uint32_t normal_str;
  uint32_t shift_str;
  uint32_t ctrl_str;
  uint32_t shift_ctrl_str;
} kVirtualKey[] = {
#define SW(s) W4(s "\0\0")
#define W4(s) (s[3] + 0u) << 24 | s[2] << 16 | s[1] << 8 | s[0]
#define VK(vk, normal_str, shift_str, ctrl_str, shift_ctrl_str) \
  { vk, SW(normal_str), SW(shift_str), SW(ctrl_str), SW(shift_ctrl_str) }
    VK(kNtVkInsert, "2~", "2;2~", "2;5~", "2;6~"),
    VK(kNtVkEnd, "4~", "4;2~", "4;5~", "4;6~"),
    VK(kNtVkDown, "B", "1;2B", "1;5B", "1;6B"),
    VK(kNtVkNext, "6~", "6;2~", "6;5~", "6;6~"),
    VK(kNtVkLeft, "D", "1;2D", "1;5D", "1;6D"),
    VK(kNtVkClear, "G", "1;2G", "1;5G", "1;6G"),
    VK(kNtVkRight, "C", "1;2C", "1;5C", "1;6C"),
    VK(kNtVkUp, "A", "1;2A", "1;5A", "1;6A"),
    VK(kNtVkHome, "1~", "1;2~", "1;5~", "1;6~"),
    VK(kNtVkPrior, "5~", "5;2~", "5;5~", "5;6~"),
    VK(kNtVkDelete, "3~", "3;2~", "3;5~", "3;6~"),
    VK(kNtVkNumpad0, "2~", "2;2~", "2;5~", "2;6~"),
    VK(kNtVkNumpad1, "4~", "4;2~", "4;5~", "4;6~"),
    VK(kNtVkNumpad2, "B", "1;2B", "1;5B", "1;6B"),
    VK(kNtVkNumpad3, "6~", "6;2~", "6;5~", "6;6~"),
    VK(kNtVkNumpad4, "D", "1;2D", "1;5D", "1;6D"),
    VK(kNtVkNumpad5, "G", "1;2G", "1;5G", "1;6G"),
    VK(kNtVkNumpad6, "C", "1;2C", "1;5C", "1;6C"),
    VK(kNtVkNumpad7, "A", "1;2A", "1;5A", "1;6A"),
    VK(kNtVkNumpad8, "1~", "1;2~", "1;5~", "1;6~"),
    VK(kNtVkNumpad9, "5~", "5;2~", "5;5~", "5;6~"),
    VK(kNtVkDecimal, "3~", "3;2~", "3;5~", "3;6~"),
    VK(kNtVkF1, "[A", "23~", "11^", "23^"),
    VK(kNtVkF2, "[B", "24~", "12^", "24^"),
    VK(kNtVkF3, "[C", "25~", "13^", "25^"),
    VK(kNtVkF4, "[D", "26~", "14^", "26^"),
    VK(kNtVkF5, "[E", "28~", "15^", "28^"),
    VK(kNtVkF6, "17~", "29~", "17^", "29^"),
    VK(kNtVkF7, "18~", "31~", "18^", "31^"),
    VK(kNtVkF8, "19~", "32~", "19^", "32^"),
    VK(kNtVkF9, "20~", "33~", "20^", "33^"),
    VK(kNtVkF10, "21~", "34~", "21^", "34^"),
    VK(kNtVkF11, "23~", "23$", "23^", "23@"),
    VK(kNtVkF12, "24~", "24$", "24^", "24@"),
#undef VK
#undef W4
#undef SW
};

static textwindows int ProcessSignal(int sig, struct Fd *f) {
  if (f) {
    if (_weaken(__sig_raise)) {
      pthread_mutex_unlock(&f->lock);
      _weaken(__sig_raise)(sig, SI_KERNEL);
      pthread_mutex_lock(&f->lock);
      if (!(__sighandflags[sig] & SA_RESTART)) {
        return eintr();
      }
    } else if (sig != SIGWINCH) {
      TerminateThisProcess(sig);
    }
  }
  return 0;
}

static textwindows uint32_t GetVirtualKey(uint16_t vk, bool shift, bool ctrl) {
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

static textwindows int ProcessKeyEvent(const struct NtInputRecord *r, char *p,
                                       uint16_t *utf16hs, struct Fd *f) {

  uint16_t c = r->Event.KeyEvent.uChar.UnicodeChar;
  uint16_t vk = r->Event.KeyEvent.wVirtualKeyCode;
  uint16_t cks = r->Event.KeyEvent.dwControlKeyState;

  // ignore keyup events
  if (!r->Event.KeyEvent.bKeyDown && (!c || vk != kNtVkMenu)) {
    return 0;
  }

  // ignore numpad being used to compose a character
  if ((cks & kNtLeftAltPressed) && !(cks & kNtEnhancedKey) &&
      (vk == kNtVkInsert || vk == kNtVkEnd || vk == kNtVkDown ||
       vk == kNtVkNext || vk == kNtVkLeft || vk == kNtVkClear ||
       vk == kNtVkRight || vk == kNtVkHome || vk == kNtVkUp ||
       vk == kNtVkPrior || vk == kNtVkNumpad0 || vk == kNtVkNumpad1 ||
       vk == kNtVkNumpad2 || vk == kNtVkNumpad3 || vk == kNtVkNumpad4 ||
       vk == kNtVkNumpad5 || vk == kNtVkNumpad6 || vk == kNtVkNumpad7 ||
       vk == kNtVkNumpad8 || vk == kNtVkNumpad9)) {
    return 0;
  }

  int n = 0;

  // process virtual keys
  if (!c) {
    uint32_t w;
    w = GetVirtualKey(vk, !!(cks & kNtShiftPressed),
                      !!(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)));
    if (!w) return 0;
    p[n++] = 033;
    if (cks & (kNtLeftAltPressed | kNtRightAltPressed)) {
      p[n++] = 033;
    }
    p[n++] = '[';
    do p[n++] = w;
    while ((w >>= 8));
    return n;
  }

  // translate utf-16 into utf-32
  if (IsHighSurrogate(c)) {
    *utf16hs = c;
    return 0;
  }
  if (IsLowSurrogate(c)) {
    c = MergeUtf16(*utf16hs, c);
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
      return ProcessSignal(SIGINT, f);
    } else if (c == __vquit && __vquit != _POSIX_VDISABLE) {
      return ProcessSignal(SIGQUIT, f);
    }
  }

  // handle ctrl-d the end of file keystroke
  if (c == __veof && __veof != _POSIX_VDISABLE) {
    return enodata();
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
static textwindows int ProcessMouseEvent(const struct NtInputRecord *r, char *b,
                                         struct Fd *f) {
  int e = 0;
  char *p = b;
  uint32_t currentbs = f ? f->mousebuttons : 0;
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
    if (f) {
      f->mousebuttons = bs;
    }
  }
  return p - b;
}

textwindows int ConvertConsoleInputToAnsi(const struct NtInputRecord *r,
                                          char p[hasatleast 32],
                                          uint16_t *utf16hs, struct Fd *f) {
  switch (r->EventType) {
    case kNtKeyEvent:
      return ProcessKeyEvent(r, p, utf16hs, f);
    case kNtMouseEvent:
      return ProcessMouseEvent(r, p, f);
    case kNtWindowBufferSizeEvent:
      return ProcessSignal(SIGWINCH, f);
    default:
      return 0;
  }
}

static textwindows ssize_t ReadFromWindowsConsole(struct Fd *f, void *data,
                                                  size_t size) {
  ssize_t rc;
  int e = errno;
  uint16_t utf16hs = 0;
  pthread_mutex_lock(&f->lock);
  for (;;) {
    if (f->eoftty) {
      rc = 0;
      break;
    }
    uint32_t got = MIN(size, f->buflen);
    uint32_t remain = f->buflen - got;
    if (got) memcpy(data, f->buf, got);
    if (remain) memmove(f->buf, f->buf + got, remain);
    f->buflen = remain;
    if (got) {
      rc = got;
      break;
    }
    uint32_t events_available;
    if (!GetNumberOfConsoleInputEvents(f->handle, &events_available)) {
      rc = __winerr();
      break;
    }
    if (events_available) {
      uint32_t n;
      struct NtInputRecord r;
      if (!ReadConsoleInput(f->handle, &r, 1, &n)) {
        rc = __winerr();
        break;
      }
      rc = ConvertConsoleInputToAnsi(&r, f->buf, &utf16hs, f);
      if (rc == -1) {
        if (errno == ENODATA) {
          f->eoftty = true;
          errno = e;
          rc = 0;
        }
        break;
      }
      f->buflen = rc;
    } else {
      if (f->flags & O_NONBLOCK) {
        rc = 0;
        break;
      }
      uint32_t ms = __SIG_POLL_INTERVAL_MS;
      if (__ttymagic & kFdTtyNoBlock) {
        if (!__vtime) {
          rc = 0;
          break;
        } else {
          ms = __vtime * 100;
        }
      }
      if ((rc = _check_interrupts(kSigOpRestartable))) {
        break;
      }
      struct PosixThread *pt = _pthread_self();
      pt->pt_flags |= PT_INSEMAPHORE;
      WaitForSingleObject(pt->semaphore, ms);
      pt->pt_flags &= ~PT_INSEMAPHORE;
      if (pt->abort_errno == ECANCELED) {
        rc = ecanceled();
        break;
      }
    }
  }
  pthread_mutex_unlock(&f->lock);
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
