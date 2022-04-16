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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/ipc.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/sock/ntstdin.internal.h"

/**
 * @fileoverview Pollable Standard Input for the New Technology.
 */

static textwindows uint32_t StdinWorkerThread(void *arg) {
  char buf[512];
  bool32 ok = true;
  uint32_t i, rc, got, err, wrote;
  struct NtStdinWorker w, *wp = arg;
  NTTRACE("StdinWorkerThread(%ld → %ld → %ld) pid %d tid %d", wp->reader,
          wp->writer, wp->consumer, getpid(), gettid());
  __sync_lock_release(&wp->sync);
  w = *wp;
  do {
    ok = ReadFile(w.reader, buf, sizeof(buf), &got, 0);
    /* When writing to a non-blocking, byte-mode pipe handle with
       insufficient buffer space, WriteFile returns TRUE with
       *lpNumberOfBytesWritten < nNumberOfBytesToWrite.
                                         ──Quoth MSDN WriteFile() */
    for (i = 0; ok && i < got; i += wrote) {
      ok = WriteFile(w.writer, buf + i, got - i, &wrote, 0);
    }
  } while (ok && got);
  err = GetLastError();
  if (!ok) {
    if (err == kNtErrorHandleEof || err == kNtErrorBrokenPipe ||
        err == kNtErrorNoData) {
      ok = true;
    }
  }
  NTTRACE("StdinWorkerThread(%ld → %ld → %ld) → %hhhd %u", w.reader, w.writer,
          w.consumer, err);
  return !ok;
}

/**
 * Converts read-only file descriptor to pollable named pipe.
 *
 * @param fd is open file descriptor to convert
 * @return new object on success, or 0 w/ errno
 */
textwindows struct NtStdinWorker *NewNtStdinWorker(int fd) {
  struct NtStdinWorker *w;
  NTTRACE("LaunchNtStdinWorker(%d) pid %d tid %d", fd, getpid(), gettid());
  assert(!g_fds.p[fd].worker);
  assert(__isfdopen(fd));
  if (!(w = calloc(1, sizeof(struct NtStdinWorker)))) return 0;
  w->refs = 1;
  w->sync = 1;
  w->reader = g_fds.p[fd].handle;
  if ((w->consumer = CreateNamedPipe(
           CreatePipeName(w->name),
           kNtPipeAccessInbound | kNtFileFlagOverlapped,
           kNtPipeTypeByte | kNtPipeReadmodeByte | kNtPipeRejectRemoteClients,
           1, 512, 512, 0, 0)) != -1) {
    if ((w->writer = CreateFile(w->name, kNtGenericWrite, 0, 0, kNtOpenExisting,
                                kNtFileFlagOverlapped, 0)) != -1) {
      if ((w->worker = CreateThread(0, 0, NT2SYSV(StdinWorkerThread), w, 0,
                                    &w->tid)) != -1) {
        while (__sync_lock_test_and_set(&w->sync, __ATOMIC_CONSUME)) {
          __builtin_ia32_pause();
        }
        g_fds.p[fd].handle = w->consumer;
        g_fds.p[fd].worker = w;
        return w;
      }
      CloseHandle(w->writer);
    }
    CloseHandle(w->consumer);
  }
  free(w);
  return w;
}

/**
 * References stdin worker on the New Technology.
 * @param w is non-null worker object
 * @return worker object for new fd
 */
textwindows struct NtStdinWorker *RefNtStdinWorker(struct NtStdinWorker *w) {
  __atomic_fetch_add(&w->refs, 1, __ATOMIC_RELAXED);
  return w;
}

/**
 * Dereferences stdin worker on the New Technology.
 * @param w is non-null worker object
 * @return true if ok otherwise false
 */
textwindows bool UnrefNtStdinWorker(struct NtStdinWorker *w) {
  bool ok = true;
  if (__atomic_sub_fetch(&w->refs, 1, __ATOMIC_SEQ_CST)) return true;
  if (!CloseHandle(w->consumer)) ok = false;
  if (!CloseHandle(w->writer)) ok = false;
  if (!CloseHandle(w->reader)) ok = false;
  if (!CloseHandle(w->worker)) ok = false;
  free(w);
  return ok;
}

/**
 * Runs post fork for stdin workers on the New Technology.
 */
textwindows void ForkNtStdinWorker(void) {
  for (int i = 0; i < g_fds.n; ++i) {
    if (g_fds.p[i].kind && g_fds.p[i].worker) {
      g_fds.p[i].handle = g_fds.p[i].worker->reader;
      free(g_fds.p[i].worker);
      g_fds.p[i].worker = 0;
    }
  }
}
