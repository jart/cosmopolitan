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
#include "libc/bits/popcnt.h"
#include "libc/calls/internal.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/yoink.inc"

static int GetFdsPopcnt(int nfds, fd_set *fds) {
  int i, n = 0;
  if (fds) {
    for (i = 0; i < nfds; ++i) {
      n += popcnt(fds->fds_bits[i]);
    }
  }
  return n;
}

static int FindFdByHandle(int nfds, int64_t h) {
  int i, n;
  n = MIN(nfds << 3, g_fds.n);
  for (i = 0; i < n; ++i) {
    if (h == g_fds.p[i].handle && g_fds.p[i].kind != kFdEmpty) {
      return i;
    }
  }
  return -1;
}

static struct NtFdSet *FdSetToNtFdSet(int nfds, fd_set *fds) {
  int i, j, k, n, m, fd;
  struct NtFdSet *ntfds;
  if (fds && (n = GetFdsPopcnt(nfds, fds))) {
    m = MIN(n, ARRAYLEN(ntfds->fd_array));
    ntfds = malloc(sizeof(struct NtFdSet));
    for (k = i = 0; i < nfds; ++i) {
      if (fds->fds_bits[i]) {
        for (j = 0; j < 64 && k < m; ++j) {
          if ((fds->fds_bits[i] & (1ul << j)) && i * 8 + j < g_fds.n) {
            ntfds->fd_array[k++] = g_fds.p[i * 8 + j].handle;
          }
        }
      }
    }
    ntfds->fd_count = m;
    return ntfds;
  } else {
    return NULL;
  }
}

static void NtFdSetToFdSet(int nfds, fd_set *fds, struct NtFdSet *ntfds) {
  int i, fd;
  if (ntfds) {
    for (i = 0; i < nfds; ++i) {
      fds->fds_bits[i] = 0;
    }
    for (i = 0; i < ntfds->fd_count; ++i) {
      if ((fd = FindFdByHandle(nfds, ntfds->fd_array[i])) != -1) {
        fds->fds_bits[fd >> 3] |= 1ul << (fd & 7);
      }
    }
  }
}

static struct NtTimeval *TimevalToNtTimeval(struct timeval *tv,
                                            struct NtTimeval *nttv) {
  if (tv) {
    nttv->tv_sec = tv->tv_sec;
    nttv->tv_usec = tv->tv_usec;
    return nttv;
  } else {
    return NULL;
  }
}

int select$nt(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
              struct timeval *timeout) {
  int n, rc;
  struct NtTimeval nttimeout, *nttimeoutp;
  struct NtFdSet *ntreadfds, *ntwritefds, *ntexceptfds;
  nfds = MIN(ARRAYLEN(readfds->fds_bits), ROUNDUP(nfds, 8)) >> 3;
  ntreadfds = FdSetToNtFdSet(nfds, readfds);
  ntwritefds = FdSetToNtFdSet(nfds, writefds);
  ntexceptfds = FdSetToNtFdSet(nfds, exceptfds);
  nttimeoutp = TimevalToNtTimeval(timeout, &nttimeout);
  rc = __select$nt(0, ntreadfds, ntwritefds, ntexceptfds, nttimeoutp);
  NtFdSetToFdSet(nfds, readfds, ntreadfds);
  NtFdSetToFdSet(nfds, writefds, ntwritefds);
  NtFdSetToFdSet(nfds, exceptfds, ntexceptfds);
  free(ntreadfds);
  free(ntwritefds);
  free(ntexceptfds);
  return rc;
}
