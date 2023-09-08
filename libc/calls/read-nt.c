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
#include "libc/assert.h"
#include "libc/calls/bo.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/wincrash.internal.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

__static_yoink("WinMainStdin");

#ifdef __x86_64__

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;

static textwindows void sys_read_nt_abort(int64_t handle,
                                          struct NtOverlapped *overlapped) {
  unassert(CancelIoEx(handle, overlapped) ||
           GetLastError() == kNtErrorNotFound);
}

textwindows ssize_t sys_read_nt_impl(int fd, void *data, size_t size,
                                     int64_t offset) {

  // perform the read i/o operation
  bool32 ok;
  struct Fd *f;
  uint32_t got;
  int filetype;
  int64_t handle;
  uint32_t remain;
  char *targetdata;
  uint32_t targetsize;
  bool is_console_input;
  int abort_errno = EAGAIN;
  f = g_fds.p + fd;
StartOver:
  size = MIN(size, 0x7ffff000);
  handle = __resolve_stdin_handle(f->handle);
  filetype = GetFileType(handle);
  is_console_input = g_fds.stdin.handle ? f->handle == g_fds.stdin.handle
                                        : f->handle == g_fds.p[0].handle;

  // the caller might be reading a single byte at a time. but we need to
  // be able to munge three bytes into just 1 e.g. "\342\220\200" → "\0"
  if (size && f->buflen) {
  ReturnDataFromBuffer:
    got = MIN(size, f->buflen);
    remain = f->buflen - got;
    if (got) memcpy(data, f->buf, got);
    if (remain) memmove(f->buf, f->buf + got, remain);
    f->buflen = remain;
    return got;
  }
  if (is_console_input && size && size < 3 && (__ttymagic & kFdTtyMunging)) {
    targetdata = f->buf;
    targetsize = sizeof(f->buf);
  } else {
    targetdata = data;
    targetsize = size;
  }

  if (filetype == kNtFileTypeChar || filetype == kNtFileTypePipe) {
    struct NtOverlapped overlap = {0};
    if (offset != -1) {
      // pread() and pwrite() should not be called on a pipe or tty
      return espipe();
    }
    if ((overlap.hEvent = CreateEvent(0, 0, 0, 0))) {
      // the win32 manual says it's important to *not* put &got here
      // since for overlapped i/o, we always use GetOverlappedResult
      ok = ReadFile(handle, targetdata, targetsize, 0, &overlap);
      if (!ok && GetLastError() == kNtErrorIoPending) {
        BEGIN_BLOCKING_OPERATION;
        // the i/o operation is in flight; blocking is unavoidable
        // if we're in a non-blocking mode, then immediately abort
        // if an interrupt is pending then we abort before waiting
        // otherwise wait for i/o periodically checking interrupts
        if (f->flags & O_NONBLOCK) {
          sys_read_nt_abort(handle, &overlap);
        } else if (_check_interrupts(kSigOpRestartable)) {
        Interrupted:
          abort_errno = errno;
          sys_read_nt_abort(handle, &overlap);
        } else {
          for (;;) {
            uint32_t i;
            if (g_fds.stdin.inisem) {
              ReleaseSemaphore(g_fds.stdin.inisem, 1, 0);
            }
            i = WaitForSingleObject(overlap.hEvent, __SIG_POLLING_INTERVAL_MS);
            if (i == kNtWaitTimeout) {
              if (_check_interrupts(kSigOpRestartable)) {
                goto Interrupted;
              }
            } else {
              npassert(!i);
              break;
            }
          }
        }
        ok = true;
        END_BLOCKING_OPERATION;
      }
      if (ok) {
        // overlapped is allocated on stack, so it's important we wait
        // for windows to acknowledge that it's done using that memory
        ok = GetOverlappedResult(handle, &overlap, &got, true);
        if (!ok && GetLastError() == kNtErrorIoIncomplete) {
          kprintf("you complete me\n");
          ok = true;
        }
      }
      __imp_CloseHandle(overlap.hEvent);
    } else {
      ok = false;
    }
  } else if (offset == -1) {
    // perform simple blocking file read
    ok = ReadFile(handle, targetdata, targetsize, &got, 0);
  } else {
    // perform pread()-style file read at particular file offset
    int64_t position;
    // save file pointer which windows clobbers, even for overlapped i/o
    if (!SetFilePointerEx(handle, 0, &position, SEEK_CUR)) {
      return __winerr();  // f probably isn't seekable?
    }
    struct NtOverlapped overlap = {0};
    overlap.Pointer = (void *)(uintptr_t)offset;
    ok = ReadFile(handle, targetdata, targetsize, 0, &overlap);
    if (!ok && GetLastError() == kNtErrorIoPending) ok = true;
    if (ok) ok = GetOverlappedResult(handle, &overlap, &got, true);
    // restore file pointer which windows clobbers, even on error
    unassert(SetFilePointerEx(handle, position, 0, SEEK_SET));
  }

  if (ok) {
    if (is_console_input) {
      if (__ttymagic & kFdTtyMunging) {
        switch (_weaken(__munge_terminal_input)(targetdata, &got)) {
          case DO_NOTHING:
            break;
          case DO_RESTART:
            goto StartOver;
          case DO_EINTR:
            return eintr();
          default:
            __builtin_unreachable();
        }
      }
      if (__ttymagic & kFdTtyEchoing) {
        _weaken(__echo_terminal_input)(f, targetdata, got);
      }
    }
    if (targetdata != data) {
      f->buflen = got;
      goto ReturnDataFromBuffer;
    }
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
      errno = abort_errno;
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
      if (!iov[i].iov_len) continue;
      rc = sys_read_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset);
      if (rc == -1) return -1;
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
