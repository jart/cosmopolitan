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
#include "libc/calls/calls.h"
#include "libc/calls/struct/rusage.h"
#include "libc/cosmo.h"
#include "libc/cxxabi.h"
#include "libc/intrin/cxaatexit.h"
#include "libc/intrin/iscall.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/tree.h"
#include "libc/intrin/weaken.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "third_party/dlmalloc/dlmalloc.h"

#define ALLOC_CONTAINER(e) TREE_CONTAINER(struct Alloc, elem, e)

struct Buffer {
  char *p;
  int n;
  int i;
};

struct Alloc {
  union {
    struct Tree elem;
    struct Alloc *next;
  };
  uintptr_t addr;
  unsigned count;
  bool truncated;
  uintptr_t backtrace[20];
};

static struct Allocs {
  struct Tree *root;
  struct Alloc *freelist;
  pthread_mutex_t lock;
} g_allocs;

static void FreeAlloc(struct Alloc *a) {
  a->next = g_allocs.freelist;
  g_allocs.freelist = a;
}

static struct Alloc *NewAlloc(void) {
  if (!g_allocs.freelist) {
    struct Alloc *as = mmap(0, __gransize, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (as == MAP_FAILED)
      return 0;
    size_t n = __gransize / sizeof(struct Alloc);
    while (n)
      FreeAlloc(&as[--n]);
  }
  struct Alloc *a = g_allocs.freelist;
  g_allocs.freelist = a->next;
  a->truncated = false;
  a->elem.parent = 0;
  a->elem.right = 0;
  a->elem.word = 0;
  a->count = 0;
  return a;
}

static int AllocSearch(const void *key, const struct Tree *node) {
  uintptr_t x = (uintptr_t)key;
  uintptr_t y = ALLOC_CONTAINER(node)->addr;
  return (x > y) - (x < y);
}

static int AllocCompare(const struct Tree *a, const struct Tree *b) {
  uintptr_t x = ALLOC_CONTAINER(a)->addr;
  uintptr_t y = ALLOC_CONTAINER(b)->addr;
  return (x > y) - (x < y);
}

static void TrackAlloc(struct Alloc *a) {
  tree_insert(&g_allocs.root, &a->elem, AllocCompare);
}

static void UntrackAlloc(struct Alloc *a) {
  tree_remove(&g_allocs.root, &a->elem);
}

static struct Alloc *GetAlloc(uintptr_t addr) {
  struct Tree *e;
  if ((e = tree_get(g_allocs.root, (void *)addr, AllocSearch)))
    return ALLOC_CONTAINER(e);
  return 0;
}

static void SaveBacktrace(struct Alloc *a, const struct StackFrame *sf) {
  for (; sf; sf = sf->next) {
    if (kisdangerous(sf)) {
      if (a->count < ARRAYLEN(a->backtrace))
        a->backtrace[a->count++] = -1;
      break;
    }
    if (a->count == ARRAYLEN(a->backtrace)) {
      a->truncated = true;
      break;
    }
    unsigned char *ip = (unsigned char *)sf->addr;
#ifdef __x86_64__
    // x86 advances the progrem counter before an instruction
    // begins executing. return addresses in backtraces shall
    // point to code after the call, which means addr2line is
    // going to print unrelated code unless we fixup the addr
    if (!kisdangerous(ip))
      ip -= __is_call(ip);
#endif
    a->backtrace[a->count++] = (uintptr_t)ip;
  }
}

relegated static void Append(struct Buffer *b, const char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  b->i += kvsnprintf(b->p + b->i, b->n - b->i, fmt, va);
  va_end(va);
}

relegated static bool AppendFileLine(struct Buffer *b, long addr) {
  ssize_t rc;
  char *p, *q, buf[128];
  int j, k, ws, pid, pfd[2];
  const char *debugbin = FindDebugBinary();
  if (!debugbin || !*debugbin)
    return false;
  const char *addr2line = GetAddr2linePath();
  if (!addr2line || !*addr2line)
    return false;
  if (pipe(pfd))
    return false;
  ksnprintf(buf, sizeof(buf), "%lx", addr);
  if ((pid = vfork()) == -1) {
    close(pfd[1]);
    close(pfd[0]);
    return false;
  }
  if (!pid) {
    close(pfd[0]);
    dup2(pfd[1], 1);
    close(2);
    execve(
        addr2line,
        (char *const[]){(char *)addr2line, "-pifCe", (char *)debugbin, buf, 0},
        (char *const[]){0});
    _Exit(127);
  }
  close(pfd[1]);
  // copy addr2line stdout to buffer. normally it is "file:line\n".
  // however additional lines can get created for inline functions.
  j = b->i;
  Append(b, "in ");
  k = b->i;
  while ((rc = read(pfd[0], buf, sizeof(buf))) > 0)
    Append(b, "%.*s", (int)rc, buf);
  // remove the annoying `foo.c:123 (discriminator 3)` suffixes, because
  // they break emacs, and who on earth knows what those things mean lol
  while ((p = memmem(b->p + k, b->i - k, " (discriminator ", 16)) &&
         (q = memchr(p + 16, '\n', (b->p + b->i) - (p + 16)))) {
    memmove(p, q, (b->p + b->i) - q);
    b->i -= q - p;
  }
  // mop up after the process and sanity check captured text
  close(pfd[0]);
  if (wait4(pid, &ws, 0, 0) != -1 && !ws && b->p[k] != ':' && b->p[k] != '?' &&
      b->p[b->i - 1] == '\n') {
    --b->i;  // chomp last newline
    return true;
  } else {
    b->i = j;  // otherwise reset the buffer
    return false;
  }
}

relegated static char *GetSymbolName(struct SymbolTable *st, int symbol) {
  char *str;
  static char buf[8192];
  if (!(str = __get_symbol_name(st, symbol)))
    return str;
  if (!cosmo_is_mangled(str))
    return str;
  cosmo_demangle(buf, str, sizeof(buf));
  return buf;
}

relegated static void PrintAllocBacktrace(struct Alloc *a) {
  for (size_t i = 0; i < a->count; ++i) {
    uintptr_t pc = a->backtrace[i];
    if (pc == -1) {
      kprintf(" <dangerous frame>\n");
      continue;
    }
    int addend, symbol;
    struct SymbolTable *st;
    if ((st = GetSymbolTable())) {
      if ((symbol = __get_symbol(st, pc))) {
        addend = pc - st->addr_base;
        addend -= st->symbols[symbol].x;
      } else {
        addend = 0;
      }
    } else {
      symbol = 0;
      addend = 0;
    }
    char buf[1000];
    struct Buffer b = {buf, 1000};
    Append(&b, " %lx ", pc);
    if (!AppendFileLine(&b, pc) && st) {
      Append(&b, "%s", GetSymbolName(st, symbol));
      if (addend)
        Append(&b, "%+d", addend);
    }
    Append(&b, "\n");
    klog(b.p, b.i);
  }
  if (a->truncated)
    kprintf(" <truncated>\n");
}

/**
 * Tracks memory for leak detector.
 *
 * If `p` is NULL, then this does nothing.
 *
 * If `p` is already tracked, then this does nothing.
 *
 * @param p is memory address to track
 * @return p always
 */
void *cosmo_leak_track(void *p) {
  if (p) {
    struct Alloc *a;
    pthread_mutex_lock(&g_allocs.lock);
    if (!GetAlloc((uintptr_t)p)) {
      if ((a = NewAlloc())) {
        a->addr = (uintptr_t)p;
        SaveBacktrace(a, __builtin_frame_address(0));
        TrackAlloc(a);
      }
    }
    pthread_mutex_unlock(&g_allocs.lock);
  }
  return p;
}

/**
 * Untracks memory for leak detector.
 *
 * If `p` is NULL, then this does nothing.
 *
 * If `p` isn't tracked, then this does nothing.
 *
 * @param p is memory address to untrack
 * @return p always
 */
void *cosmo_leak_untrack(void *p) {
  if (p) {
    struct Alloc *a;
    pthread_mutex_lock(&g_allocs.lock);
    if ((a = GetAlloc((uintptr_t)p))) {
      UntrackAlloc(a);
      FreeAlloc(a);
    }
    pthread_mutex_unlock(&g_allocs.lock);
  }
  return p;
}

/**
 * Runs thread and process destructors.
 *
 * If you're tracking malloc() allocations then you can use this routine
 * to ensure your atexit() callbacks are called, before you go ahead and
 * call cosmo_leak_print(). The caller needs to ensure other threads are
 * terminated before calling this.
 */
void cosmo_leak_finalize(void) {
  if (_weaken(_pthread_decimate))
    _weaken(_pthread_decimate)(kPosixThreadZombie);
  if (!pthread_orphan_np())
    kprintf("warning: called cosmo_leak_shutdown() from non-orphaned thread\n");
  __cxa_thread_finalize();
  __cxa_finalize(0);
}

/**
 * Prints error message for each memory leaks.
 *
 * You'll likely want to call cosmo_leak_shutdown() before this.
 *
 * @param sel if not null returns zero if leak addr should be ignored
 * @return number of leaks detected, or 0 for none
 */
int cosmo_leak_print(int sel(void *)) {
  int count = 0;
  struct Tree *e;
  pthread_mutex_lock(&g_allocs.lock);
  for (e = tree_first(g_allocs.root); e; e = tree_next(e)) {
    struct Alloc *a = ALLOC_CONTAINER(e);
    if (sel && !sel((void *)a->addr))
      continue;
    kprintf("error: memory leak detected %#lx\n", a->addr);
    PrintAllocBacktrace(a);
    ++count;
  }
  pthread_mutex_unlock(&g_allocs.lock);
  return count;
}
