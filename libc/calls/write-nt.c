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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;

static bool IsMouseModeCommand(int x) {
  return x == 1000 ||  // SET_VT200_MOUSE
         x == 1002 ||  // SET_BTN_EVENT_MOUSE
         x == 1006 ||  // SET_SGR_EXT_MODE_MOUSE
         x == 1015;    // SET_URXVT_EXT_MODE_MOUSE
}

static textwindows ssize_t sys_write_nt_impl(int fd, void *data, size_t size,
                                             ssize_t offset) {

  // perform the write i/o operation
  bool32 ok;
  struct Fd *f;
  uint32_t sent;
  int64_t handle;
  struct PosixThread *pt;

  f = g_fds.p + fd;
  pt = _pthread_self();
  size = MIN(size, 0x7ffff000);
  if (f->kind == kFdConsole) {
    handle = f->extra;  // get write end of console
  } else {
    handle = f->handle;
  }

  bool pwriting = offset != -1;
  bool seekable = f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk;
  bool nonblock = !!(f->flags & O_NONBLOCK);
  pt->abort_errno = EAGAIN;

  if (pwriting && !seekable) {
    return espipe();
  }
  if (!pwriting) {
    offset = 0;
  }

  // To use the tty mouse events feature:
  //   - write(1, "\e[?1000;1002;1015;1006h") to enable
  //   - write(1, "\e[?1000;1002;1015;1006l") to disable
  // See o//examples/ttyinfo.com and o//tool/viz/life.com
  uint32_t cm;
  if (!seekable && (f->kind == kFdConsole || GetConsoleMode(handle, &cm))) {
    int64_t hin;
    if (f->kind == kFdConsole) {
      hin = f->handle;
    } else {
      hin = GetStdHandle(kNtStdInputHandle);
    }
    if (GetConsoleMode(hin, &cm)) {
      int t = 0;
      unsigned x;
      bool m = false;
      char *p = data;
      uint32_t cm2 = cm;
      for (int i = 0; i < size; ++i) {
        switch (t) {
          case 0:
            if (p[i] == 033) {
              t = 1;
            }
            break;
          case 1:
            if (p[i] == '[') {
              t = 2;
            } else {
              t = 0;
            }
            break;
          case 2:
            if (p[i] == '?') {
              t = 3;
              x = 0;
            } else {
              t = 0;
            }
            break;
          case 3:
            if ('0' <= p[i] && p[i] <= '9') {
              x *= 10;
              x += p[i] - '0';
            } else if (p[i] == ';') {
              m |= IsMouseModeCommand(x);
              x = 0;
            } else {
              m |= IsMouseModeCommand(x);
              if (p[i] == 'h') {
                __ttymagic |= kFdTtyXtMouse;
                cm2 |= kNtEnableMouseInput;
                cm2 &= kNtEnableQuickEditMode;  // precludes mouse events
              } else if (p[i] == 'l') {
                __ttymagic &= ~kFdTtyXtMouse;
                cm2 |= kNtEnableQuickEditMode;  // disables mouse too
              }
              t = 0;
            }
            break;
          default:
            __builtin_unreachable();
        }
      }
      if (cm2 != cm) {
        SetConsoleMode(hin, cm2);
      }
    }
  }

  if (seekable && !pwriting) {
    pthread_mutex_lock(&f->lock);
    offset = f->pointer;
  }

  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 0, 0, 0),
                                 .Pointer = offset};
  ok = WriteFile(handle, data, size, 0, &overlap);
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
    ok = GetOverlappedResult(handle, &overlap, &sent, nonblock);
    if (!ok && GetLastError() == kNtErrorIoIncomplete) {
      goto BlockingOperation;
    }
  }
  __imp_CloseHandle(overlap.hEvent);  // __imp_ to avoid log noise

  if (seekable && !pwriting) {
    if (ok) f->pointer = offset + sent;
    pthread_mutex_unlock(&f->lock);
  }

  if (ok) {
    return sent;
  }

  errno_t err;
  if (_weaken(pthread_testcancel_np) &&
      (err = _weaken(pthread_testcancel_np)())) {
    return ecanceled();
  }

  switch (GetLastError()) {
    // case kNtErrorInvalidHandle:
    //   return ebadf(); /* handled by consts.sh */
    // case kNtErrorNotEnoughQuota:
    //   return edquot(); /* handled by consts.sh */
    case kNtErrorBrokenPipe:  // broken pipe
    case kNtErrorNoData:      // closing named pipe
      if (_weaken(__sig_raise)) {
        _weaken(__sig_raise)(SIGPIPE, SI_KERNEL);
        return epipe();
      } else {
        TerminateThisProcess(SIGPIPE);
      }
    case kNtErrorAccessDenied:  // write doesn't return EACCESS
      return ebadf();
    case kNtErrorOperationAborted:
      errno = pt->abort_errno;
      return -1;
    default:
      return __winerr();
  }
}

textwindows ssize_t sys_write_nt(int fd, const struct iovec *iov, size_t iovlen,
                                 ssize_t opt_offset) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1) return einval();
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len) continue;
      rc = sys_write_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset);
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
    return sys_write_nt_impl(fd, NULL, 0, opt_offset);
  }
}
