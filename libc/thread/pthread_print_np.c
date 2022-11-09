/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/dll.h"

#define N 2048
#define M 15

#define append(f, ...) o += f(buf + o, N - o, __VA_ARGS__)

static const char *DescribeStatus(enum PosixThreadStatus status) {
  switch (status) {
    case kPosixThreadJoinable:
      return "JOINAB";
    case kPosixThreadDetached:
      return "DETACH";
    case kPosixThreadTerminated:
      return "TERMIN";
    case kPosixThreadZombie:
      return "ZOMBIE";
    default:
      unreachable;
  }
}

static const char *DescribeFlags(char buf[M], struct PosixThread *pt) {
  char *p = buf;
  if (pt->cancelled) *p++ = '*';
  if (pt->flags & PT_EXITING) *p++ = 'X';
  if (pt->flags & PT_STATIC) *p++ = 'S';
  if (pt->flags & PT_OWNSTACK) *p++ = 'O';
  if (pt->flags & PT_ASYNC) *p++ = 'A';
  if (pt->flags & PT_MASKED) *p++ = 'M';
  if (pt->flags & PT_OPENBSD_KLUDGE) *p++ = 'K';
  if (pt->flags & PT_INCANCEL) *p++ = '?';
  if (pt->flags & PT_NOCANCEL) *p++ = '!';
  *p = 0;
  return buf;
}

int pthread_print_np(int fd, const char *fmt, ...) {
  va_list va;
  int rc, o = 0;
  nsync_dll_element_ *e;
  struct PosixThread *pt;
  char buf[N], flagbuf[M];
  pthread_spin_lock(&_pthread_lock);
  if (fmt) {
    va_start(va, fmt);
    append(kvsnprintf, fmt, va);
    va_end(va);
    append(ksnprintf, "\n");
  }
  append(ksnprintf, "%6s %6s %6s %6s %s\n", "ptid", "tid", "status", "flags",
         "start");
  for (e = nsync_dll_first_(_pthread_list); e;
       e = nsync_dll_next_(_pthread_list, e)) {
    pt = (struct PosixThread *)e->container;
    append(ksnprintf, "%-6d %-6d %6s %6s %t\n", pt->ptid, pt->tib->tib_tid,
           DescribeStatus(pt->status), DescribeFlags(flagbuf, pt), pt->start);
  }
  pthread_spin_unlock(&_pthread_lock);
  BLOCK_CANCELLATIONS;
  strace_enabled(-1);
  rc = write(fd, buf, strlen(buf));
  strace_enabled(+1);
  ALLOW_CANCELLATIONS;
  return rc;
}
