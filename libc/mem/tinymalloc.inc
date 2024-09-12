// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/mem.h"
#include "libc/stdalign.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"

#ifndef TINYMALLOC_MAX_BYTES
#define TINYMALLOC_MAX_BYTES 1073741824
#endif

#ifndef TINYMALLOC_MAX_ALIGN
#define TINYMALLOC_MAX_ALIGN sizeof(max_align_t)
#endif

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#pragma GCC diagnostic ignored "-Wanalyzer-use-after-free"

static struct {
  alignas(max_align_t) char bits[TINYMALLOC_MAX_BYTES];
  char *memory;
  int once;
  size_t size, used, last, free;
} heap;

static void tinymalloc_init(void) {
  int align;
  if (heap.once)
    return;
  align = TINYMALLOC_MAX_ALIGN;
  heap.memory = (char *)(((uintptr_t)heap.bits + align - 1) & -align);
  heap.size = sizeof(heap.bits) - (heap.memory - heap.bits);
  heap.once = 1;
}

__attribute__((__destructor__)) static void destroy(void) {
  kprintf("used = %'zu\n", heap.used);
}

static inline int isheap(char *mem) {
  return heap.memory <= mem && mem < heap.memory + heap.used;
}

void free(void *ptr) {
  char *mem;
  size_t base;
  if (ptr) {
    mem = (char *)ptr;
    unassert(isheap(mem));
    base = mem - heap.memory;
    *(size_t *)mem = heap.free;
    heap.free = base;
  }
}

size_t malloc_usable_size(void *ptr) {
  char *mem = (char *)ptr;
  unassert(isheap(mem));
  return ((size_t *)mem)[-1];
}

void *memalign(size_t align, size_t need) {
  char *res;
  size_t next, next2, base, toto, *link, *link2;
  tinymalloc_init();

  // normalize arguments
  while (align & (align - 1))
    ++align;
  if (need < sizeof(size_t))
    need = sizeof(size_t);
  if (align < sizeof(size_t))
    align = sizeof(size_t);
  if (align > TINYMALLOC_MAX_ALIGN)
    goto InvalidArgument;
  // TODO(jart): refactor append*() to not need size_t*2 granularity
  if (ckd_add(&need, need, sizeof(size_t) * 2 - 1))
    goto OutOfMemory;
  need &= -sizeof(size_t);

  // allocate from free list
  next = heap.free;
  link = &heap.free;
  while (next) {
    next2 = *(size_t *)(heap.memory + next);
    link2 = (size_t *)(heap.memory + next);
    if (need <= ((size_t *)(heap.memory + next))[-1]) {
      *link = next2;
      return (void *)(heap.memory + next);
    }
    next = next2;
    link = link2;
  }

  // allocate new static memory
  base = heap.used;
  base += sizeof(size_t);
  base += align - 1;
  base &= -align;
  if (ckd_add(&toto, base, need))
    goto OutOfMemory;
  if (toto > heap.size)
    goto OutOfMemory;
  res = heap.memory + base;
  ((size_t *)res)[-1] = need;
  heap.used = toto;
  heap.last = base;
  return res;

  // we require more vespene gas
OutOfMemory:
  errno = ENOMEM;
  return 0;
InvalidArgument:
  errno = EINVAL;
  return 0;
}

void *malloc(size_t need) {
  return memalign(sizeof(max_align_t), need);
}

void *calloc(size_t count, size_t size) {
  char *res;
  size_t need, used;
  if (ckd_mul(&need, count, size))
    need = -1;
  used = heap.used;
  if ((res = (char *)malloc(need)))
    if (res - heap.memory < used)
      bzero(res, need);
  return res;
}

void *realloc(void *ptr, size_t need) {
  char *res, *mem;
  size_t base, have, toto;
  if (!ptr) {
    res = (char *)malloc(need);
  } else {
    mem = (char *)ptr;
    unassert(isheap(mem));
    have = ((size_t *)mem)[-1];
    base = mem - heap.memory;
    if (need < have) {
      res = mem;
    } else if (base == heap.last) {
      if (need < sizeof(size_t))
        need = sizeof(size_t);
      if (ckd_add(&need, need, sizeof(size_t) - 1))
        goto OutOfMemory;
      need &= -sizeof(size_t);
      if (ckd_add(&toto, base, need))
        goto OutOfMemory;
      if (toto > heap.size)
        goto OutOfMemory;
      ((size_t *)mem)[-1] = need;
      heap.used = toto;
      res = mem;
    } else if ((res = (char *)malloc(need))) {
      if (have > need)
        have = need;
      memcpy(res, mem, have);
      free(mem);
    }
  }
  return res;
OutOfMemory:
  errno = ENOMEM;
  return 0;
}

#pragma GCC pop_options
