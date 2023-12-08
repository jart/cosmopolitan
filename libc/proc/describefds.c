/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/sysv/consts/o.h"

#define FDS_VAR "_COSMO_FDS="

#define MAX_ENTRY_BYTES 256

/**
 * @fileoverview fd/handle inheritance for execve() and posix_spawn()
 */

struct StringBuilder {
  char *p;
  int i, n;
};

// returns true if fd can't be inherited by anything
textwindows bool __is_cloexec(const struct Fd *f) {
  if (f->kind == kFdEmpty) return true;
  if (f->kind == kFdReserved) return true;
  if (f->kind == kFdZip) return true;
  if (f->kind == kFdEpoll) return true;
  if (f->flags & O_CLOEXEC) return true;
  if (f->handle == -1) return true;
  if (!f->handle) return true;
  return false;
}

// this must be called after ntspawn() returns
// we perform critical cleanup that _exit() can't do
textwindows void __undescribe_fds(int64_t hCreatorProcess,
                                  int64_t *lpExplicitHandles,
                                  uint32_t dwExplicitHandleCount) {
  if (lpExplicitHandles) {
    for (uint32_t i = 0; i < dwExplicitHandleCount; ++i) {
      DuplicateHandle(hCreatorProcess, lpExplicitHandles[i], 0, 0, 0, false,
                      kNtDuplicateCloseSource);
    }
    free(lpExplicitHandles);
  }
}

// serializes file descriptors and generates child handle array
// 1. serialize file descriptor table to environment variable str
// 2. generate array that'll tell CreateProcess() what to inherit
textwindows char *__describe_fds(const struct Fd *fds, size_t fdslen,
                                 struct NtStartupInfo *lpStartupInfo,
                                 int64_t hCreatorProcess,
                                 int64_t **out_lpExplicitHandles,
                                 uint32_t *out_lpExplicitHandleCount) {
  char *b, *p;
  uint32_t hi = 0;
  struct StringBuilder sb;
  int64_t *handles, handle;
  uint32_t handlecount = 0;

  // setup memory for environment variable
  if (!(sb.p = strdup(FDS_VAR))) return 0;
  sb.i = sizeof(FDS_VAR) - 1;
  sb.n = sizeof(FDS_VAR);

  // setup memory for explicitly inherited handle list
  for (int fd = 0; fd < fdslen; ++fd) {
    const struct Fd *f = fds + fd;
    if (__is_cloexec(f)) continue;
    ++handlecount;
  }
  if (!(handles = calloc(handlecount, sizeof(*handles)))) {
  OnFailure:
    __undescribe_fds(hCreatorProcess, handles, hi);
    free(sb.p);
    return 0;
  }

  // serialize file descriptors
  for (int fd = 0; fd < fdslen; ++fd) {
    const struct Fd *f = fds + fd;
    if (__is_cloexec(f)) continue;

    // make inheritable version of handle exist in creator process
    if (!DuplicateHandle(GetCurrentProcess(), f->handle, hCreatorProcess,
                         &handle, 0, true, kNtDuplicateSameAccess)) {
      STRACE("__describe_fds() DuplicateHandle() failed w/ %d", GetLastError());
      __winerr();
      goto OnFailure;
    }
    for (uint32_t i = 0; i < 3; ++i) {
      if (lpStartupInfo->stdiofds[i] == f->handle) {
        lpStartupInfo->stdiofds[i] = handle;
      }
    }
    handles[hi++] = handle;

    // ensure output string has enough space for new entry
    if (sb.i + MAX_ENTRY_BYTES > sb.n) {
      char *p2;
      sb.n += sb.n >> 1;
      sb.n += MAX_ENTRY_BYTES;
      if ((p2 = realloc(sb.p, sb.n))) {
        sb.p = p2;
      } else {
        goto OnFailure;
      }
    }

    // serialize file descriptor
    p = b = sb.p + sb.i;
    p = FormatInt64(p, fd);
    *p++ = '_';
    p = FormatInt64(p, handle);
    *p++ = '_';
    p = FormatInt64(p, f->kind);
    *p++ = '_';
    p = FormatInt64(p, f->flags);
    *p++ = '_';
    p = FormatInt64(p, f->mode);
    *p++ = '_';
    p = FormatInt64(p, f->pointer);
    *p++ = '_';
    p = FormatInt64(p, f->type);
    *p++ = '_';
    p = FormatInt64(p, f->family);
    *p++ = '_';
    p = FormatInt64(p, f->protocol);
    *p++ = ';';
    unassert(p - b < MAX_ENTRY_BYTES);
    sb.i += p - b;
    *p = 0;
  }

  // return result
  *out_lpExplicitHandles = handles;
  *out_lpExplicitHandleCount = hi;
  unassert(hi == handlecount);
  return sb.p;
}
