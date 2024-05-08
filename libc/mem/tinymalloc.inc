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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"

#ifndef TINYMALLOC_MAX_BYTES
#define TINYMALLOC_MAX_BYTES 1073741824
#endif

#ifndef TINYMALLOC_MAX_ALIGN
#define TINYMALLOC_MAX_ALIGN 4096
#endif

#ifndef MODE_DBG /* don't interfere with asan dlmalloc hooking */

_Alignas(TINYMALLOC_MAX_ALIGN) static struct {
  char memory[TINYMALLOC_MAX_BYTES];
  unsigned used, last, free;
} heap;

static inline bool isheap(char *mem) {
  return heap.memory <= mem && mem < heap.memory + heap.used;
}

void free(void *ptr) {
  char *mem;
  unsigned base;
  if (ptr) {
    mem = (char *)ptr;
    unassert(isheap(mem));
    base = mem - heap.memory;
    *(unsigned *)mem = heap.free;
    heap.free = base;
  }
}

size_t malloc_usable_size(void *ptr) {
  char *mem = (char *)ptr;
  unassert(isheap(mem));
  return ((unsigned *)mem)[-1];
}

void *memalign(size_t align, size_t need) {
  char *res;
  unsigned next, next2, base, toto, *link, *link2;

  // normalize arguments
  while (align & (align - 1))
    ++align;
  if (need < sizeof(unsigned))
    need = sizeof(unsigned);
  if (align < sizeof(unsigned))
    align = sizeof(unsigned);
  if (align > TINYMALLOC_MAX_ALIGN)
    goto InvalidArgument;
  if (ckd_add(&need, need, sizeof(unsigned) - 1))
    goto OutOfMemory;
  need &= -sizeof(unsigned);

  // allocate from free list
  next = heap.free;
  link = &heap.free;
  while (next) {
    next2 = *(unsigned *)(heap.memory + next);
    link2 = (unsigned *)(heap.memory + next);
    if (need <= ((unsigned *)(heap.memory + next))[-1]) {
      *link = next2;
      return (void *)(heap.memory + next);
    }
    next = next2;
    link = link2;
  }

  // allocate new static memory
  base = heap.used;
  base += sizeof(unsigned);
  base += align - 1;
  base &= -align;
  if (ckd_add(&toto, base, need))
    goto OutOfMemory;
  if (toto > TINYMALLOC_MAX_BYTES)
    goto OutOfMemory;
  res = heap.memory + base;
  ((unsigned *)res)[-1] = need;
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
  unsigned need, used;
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
  unsigned base, have, toto;
  if (!ptr) {
    res = (char *)malloc(need);
  } else {
    mem = (char *)ptr;
    unassert(isheap(mem));
    have = ((unsigned *)mem)[-1];
    base = mem - heap.memory;
    if (need < have) {
      res = mem;
    } else if (base == heap.last) {
      if (need < sizeof(unsigned))
        need = sizeof(unsigned);
      if (ckd_add(&need, need, sizeof(unsigned) - 1))
        goto OutOfMemory;
      need &= -sizeof(unsigned);
      if (ckd_add(&toto, base, need))
        goto OutOfMemory;
      if (toto > TINYMALLOC_MAX_BYTES)
        goto OutOfMemory;
      ((unsigned *)mem)[-1] = need;
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

#endif /* MODE_DBG */
