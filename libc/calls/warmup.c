/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/processcreationflags.h"
#include "libc/nt/enum/threadpriority.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

#define MAX_THREADS 3

#define STACK_SIZE 65536

#define ABI __msabi textwindows dontinstrument

#pragma GCC diagnostic ignored "-Wframe-larger-than="

__msabi extern typeof(CloseHandle) *const __imp_CloseHandle;
__msabi extern typeof(CreateThread) *const __imp_CreateThread;
__msabi extern typeof(FindClose) *const __imp_FindClose;
__msabi extern typeof(FindFirstFile) *const __imp_FindFirstFileW;
__msabi extern typeof(FindNextFile) *const __imp_FindNextFileW;
__msabi extern typeof(GetProcessHeap) *const __imp_GetProcessHeap;
__msabi extern typeof(HeapAlloc) *const __imp_HeapAlloc;
__msabi extern typeof(HeapFree) *const __imp_HeapFree;

struct Dir {
  struct Dir *next;
  bool isdirectory;
  char16_t path[];
};

struct Crawler {
  int threads;
  int entries;
  bool isrecursive;
  struct Dir *queue;
  pthread_mutex_t lock;
};

ABI static void *Malloc(size_t size) {
  return __imp_HeapAlloc(__imp_GetProcessHeap(), 0, size);
}

ABI static void Free(void *ptr) {
  __imp_HeapFree(__imp_GetProcessHeap(), 0, ptr);
}

ABI static uint32_t cosmo_warmup_directory_worker(void *arg) {
  struct CosmoTib tls;
  __bootstrap_tls(&tls, __builtin_frame_address(0));
  SetThreadPriority(GetCurrentThread(), kNtThreadPriorityIdle);
  struct Crawler *crawler = arg;
  for (;;) {
    struct Dir *dir = 0;
    pthread_mutex_lock(&crawler->lock);
    if (crawler->queue) {
      dir = crawler->queue;
      crawler->queue = dir->next;
      --crawler->entries;
    } else {
      --crawler->threads;
    }
    pthread_mutex_unlock(&crawler->lock);
    if (!dir) {
      if (!crawler->threads)
        Free(crawler);
      for (int i = 0; i < ARRAYLEN(tls.tib_events); ++i)
        if (tls.tib_events[i])
          __imp_CloseHandle(tls.tib_events[i]);
      return 0;
    }
    char16_t path[PATH_MAX];
    int baselen = strlcpy16(path, dir->path, PATH_MAX);
    bool isdirectory = dir->isdirectory;
    Free(dir);
    if (!isdirectory) {
      char path8[PATH_MAX];
      if (__mkunixpath(path, path8) != -1) {
        struct stat st;
        sys_fstatat_nt(AT_FDCWD, path8, &st, AT_SYMLINK_NOFOLLOW);
      }
    } else {
      if (baselen + 2 + 1 > PATH_MAX)
        continue;
      if (path[baselen - 1] != '\\')
        path[baselen++] = '\\';
      path[baselen] = '*';
      path[baselen + 1] = 0;
      intptr_t hDirectory;
      struct NtWin32FindData dent;
      if ((hDirectory = __imp_FindFirstFileW(path, &dent)) != -1) {
        do {
          size_t sublen = strlen16(dent.cFileName);
          if (sublen == 1 && dent.cFileName[0] == '.')
            continue;
          if (sublen == 2 &&
              (dent.cFileName[0] == '.' && dent.cFileName[1] == '.'))
            continue;
          if (baselen + sublen + 1 > PATH_MAX)
            continue;
          memcpy(path + baselen, dent.cFileName, (sublen + 1) * 2);
          struct Dir *entry;
          int len = baselen + sublen;
          bool isdirectory =
              (dent.dwFileAttributes & kNtFileAttributeDirectory) &&
              !(dent.dwFileAttributes & kNtFileAttributeReparsePoint);
          if (crawler->isrecursive || !isdirectory) {
            if ((entry = Malloc(sizeof(struct Dir) + (len + 1) * 2))) {
              memcpy(entry->path, path, (len + 1) * 2);
              pthread_mutex_lock(&crawler->lock);
              ++crawler->entries;
              entry->isdirectory = isdirectory;
              entry->next = crawler->queue;
              crawler->queue = entry;
              if (crawler->entries > 10 && crawler->threads < MAX_THREADS) {
                intptr_t hThread = __imp_CreateThread(
                    0, STACK_SIZE, cosmo_warmup_directory_worker, crawler,
                    kNtStackSizeParamIsAReservation, 0);
                if (hThread) {
                  __imp_CloseHandle(hThread);
                  ++crawler->threads;
                }
              }
              pthread_mutex_unlock(&crawler->lock);
            }
          }
        } while (__imp_FindNextFileW(hDirectory, &dent));
        __imp_FindClose(hDirectory);
      }
    }
  }
}

/**
 * Pulls directory metadata into filesystem cache.
 *
 * Using functions like nftw() to traverse large directories on Windows
 * is normally very slow, because (1) our readdir() implementation has
 * to call CreateFile() each time in order to get the inode, and (2) our
 * fstatat() implementation needs to read the first two bytes of a file
 * to determine if it's an executable.
 *
 * However this alone shouldn't be the cause for much concern. The root
 * cause is that the Windows filesystem is laugh out loud slow. This is
 * true even if you're using a so-called "dev drive" that has Defender
 * disabled. It's worse when filesystem operations actually hit disk.
 * For example, with an NVME M2 dev drive, it takes nftw() a minute to
 * crawl a directory with 18k files when the cache is cold, but if the
 * Windows filesystem cache is warm, then it takes 185ms.
 *
 * This function helps nftw() go 10x faster because it calls this which
 * spawns 32 threads in the background to prefill the filesystem cache.
 * That brings our cold cache crawl time down from 52 to 3.5 seconds!
 */
textwindows void cosmo_warmup_directory(const char *path, int isrecursive) {
  int path16len;
  char16_t path16[PATH_MAX];
  if (!IsWindows())
    return;
  if ((path16len = __mkntpath(path, path16)) != -1) {
    struct Crawler *crawler;
    if ((crawler = Malloc(sizeof(struct Crawler))) &&
        (crawler->queue = Malloc(sizeof(struct Dir) + (path16len + 1) * 2))) {
      crawler->threads = 1;
      crawler->entries = 1;
      crawler->isrecursive = isrecursive;
      crawler->queue->next = 0;
      crawler->queue->isdirectory = true;
      crawler->lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
      memcpy(crawler->queue->path, path16, (path16len + 1) * 2);
      intptr_t hThread =
          __imp_CreateThread(0, STACK_SIZE, cosmo_warmup_directory_worker,
                             crawler, kNtStackSizeParamIsAReservation, 0);
      if (hThread)
        __imp_CloseHandle(hThread);
    }
  }
}
