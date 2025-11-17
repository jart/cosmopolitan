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
#include "libc/calls/calls.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/maps.h"
#include "libc/nt/enum/heap.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/proc/describefds.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

#define FDS_VAR "_COSMO_FDS_V2="

/**
 * @fileoverview fd/handle inheritance for execve() and posix_spawn()
 */

struct String {
  long n;
  long c;
  char *p;
};

struct Longs {
  long n;
  long c;
  long *p;
};

textwindows static void *Malloc(size_t size) {
  return HeapAlloc(GetProcessHeap(), 0, size);
}

textwindows static void *Calloc(size_t nmemb, size_t size) {
  return HeapAlloc(GetProcessHeap(), kNtHeapZeroMemory, nmemb * size);
}

textwindows static void *Realloc(void *addr, size_t size) {
  return HeapReAlloc(GetProcessHeap(), 0, addr, size);
}

textwindows static void Free(void *addr) {
  HeapFree(GetProcessHeap(), 0, addr);
}

textwindows static int InitString(struct String *sb) {
  if (!(sb->p = Malloc(16)))
    return enomem();
  sb->p[0] = 0;
  sb->n = 0;
  sb->c = 16;
  return 0;
}

textwindows static int InitLongs(struct Longs *hl) {
  if (!(hl->p = Malloc(sizeof(long) * 2)))
    return enomem();
  hl->p[0] = 0;
  hl->n = 0;
  hl->c = 2;
  return 0;
}

textwindows static int AppendString(struct String *sb, const char *s) {
  long n = strlen(s);
  if (sb->n + n + 1 > sb->c) {
    long c2 = sb->c;
    do {
      c2 += 1;
      c2 += c2 >> 1;
    } while (sb->n + n + 1 > c2);
    char *p2 = Realloc(sb->p, c2);
    if (!p2)
      return enomem();
    sb->p = p2;
    sb->c = c2;
  }
  memcpy(sb->p + sb->n, s, n + 1);
  sb->n += n;
  return 0;
}

textwindows static int AppendLongToString(struct String *sb, long x) {
  char buf[21];
  FormatInt64(buf, x);
  return AppendString(sb, buf);
}

textwindows static int AppendLong(struct Longs *hl, long h) {
  if (hl->n + 2 > hl->c) {
    long c2 = hl->c;
    c2 += 2;
    c2 += c2 >> 1;
    long *p2 = Realloc(hl->p, c2 * sizeof(long));
    if (!p2)
      return enomem();
    hl->p = p2;
    hl->c = c2;
  }
  hl->p[hl->n++] = h;
  hl->p[hl->n] = 0;
  return 0;
}

textwindows static int AppendHandle(struct Longs *hl, long hObject,
                                    long hProcess,
                                    long *opt_out_hDuplicateObject) {
  long hDuplicateObject;
  if (!DuplicateHandle(GetCurrentProcess(), hObject, hProcess,
                       &hDuplicateObject, 0, true, kNtDuplicateSameAccess))
    return __winerr();
  if (AppendLong(hl, hDuplicateObject) == -1) {
    CloseHandle(hDuplicateObject);
    return -1;
  }
  if (opt_out_hDuplicateObject)
    *opt_out_hDuplicateObject = hDuplicateObject;
  return 0;
}

// returns true if fd can't be inherited by anything
textwindows bool __is_cloexec(const struct Fd *f) {
  if (f->kind == kFdEmpty)
    return true;
  if (f->kind == kFdReserved)
    return true;
  if (f->kind == kFdZip)
    return true;
  if (f->kind == kFdEpoll)
    return true;
  if (f->flags & O_CLOEXEC)
    return true;
  if (f->handle == -1)
    return true;
  if (!f->handle)
    return true;
  return false;
}

// this must be called after ntspawn() returns
// we perform critical cleanup that _exit() can't do
textwindows void __undescribe_fds(char *fdSpecEnvVariable, long hCreatorProcess,
                                  long *lpExplicitHandles,
                                  uint32_t dwExplicitHandleCount) {
  if (lpExplicitHandles) {
    for (uint32_t i = 0; i < dwExplicitHandleCount; ++i)
      DuplicateHandle(hCreatorProcess, lpExplicitHandles[i], 0, 0, 0, false,
                      kNtDuplicateCloseSource);
    Free(lpExplicitHandles);
  }
  Free(fdSpecEnvVariable);
}

// serializes file descriptors and generates child handle array
// 1. serialize file descriptor table to environment variable str
// 2. generate array that'll tell CreateProcess() what to inherit
textwindows char *__describe_fds(const struct Fd *fds, size_t fdslen,
                                 long *opt_inout_lpExtraHandles,
                                 struct NtStartupInfo *lpStartupInfo,
                                 long hCreatorProcess,
                                 long **out_lpExplicitHandles,
                                 uint32_t *out_lpExplicitHandleCount) {
  long handle;
  struct Longs handles = {0};
  struct String envstr = {0};

  if (InitLongs(&handles))
    goto OnError;
  if (InitString(&envstr))
    goto OnError;
  if (AppendString(&envstr, FDS_VAR))
    goto OnError;

  // misc handles to inherit
  if (opt_inout_lpExtraHandles) {
    while (*opt_inout_lpExtraHandles) {
      if (AppendHandle(&handles, *opt_inout_lpExtraHandles, hCreatorProcess,
                       &handle))
        goto OnError;
      *opt_inout_lpExtraHandles++ = handle;
    }
  }

  // serialize file descriptors
  for (int fd = 0; fd < fdslen; ++fd) {
    const struct Fd *f = fds + fd;
    if (__is_cloexec(f))
      continue;
    if (f->cursor)
      // taint cursor so it can't be cached when freed
      f->cursor->is_multiprocess = true;

    // make inheritable version of handle exist in creator process
    if (AppendHandle(&handles, f->handle, hCreatorProcess, &handle))
      goto OnError;
    for (uint32_t i = 0; i < 3; ++i)
      if (lpStartupInfo->stdiofds[i] == f->handle)
        lpStartupInfo->stdiofds[i] = handle;

    // get shared memory handle for the file offset pointer
    intptr_t shand = 0;
    if (f->cursor) {
      struct Map *map;
      if (!(map = __maps_floor((const char *)f->cursor->shared)) ||
          map->addr != (const char *)f->cursor->shared) {
        errno = EFAULT;
        goto OnError;
      }
      if (AppendHandle(&handles, map->hand, hCreatorProcess, &shand))
        goto OnError;
    }

    // serialize file descriptors
    if (AppendLongToString(&envstr, fd))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, handle))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->kind))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->flags))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->mode))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, shand))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->type))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->family))
      goto OnError;
    if (AppendString(&envstr, "_"))
      goto OnError;
    if (AppendLongToString(&envstr, f->protocol))
      goto OnError;
    if (AppendString(&envstr, ";"))
      goto OnError;
  }

  // return result
  *out_lpExplicitHandles = handles.p;
  *out_lpExplicitHandleCount = handles.n;
  return envstr.p;

OnError:
  __undescribe_fds(envstr.p, hCreatorProcess, handles.p, handles.n);
  return 0;
}
