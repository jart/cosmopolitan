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

#include <assert.h>
#include <errno.h>
#include <stdckdint.h>
#include <stdlib.h>
#include <string.h>

// cosmopolitan "tiny" memory allocator
//
// we implement malloc() and friends using an O(n) linked list algorithm
// that does not defragment memory and supports single-threaded programs
// using .bss memory, which places a limit on how big your heap can grow
//
// you can use this allocator by including it in your main.c module file
//
//     #define TINYMALLOC_SIZE_T unsigned
//     #define TINYMALLOC_ALIGN sizeof(unsigned)
//     #include "libc/mem/tinymalloc.inc"
//
// the normal malloc implementation is very large in order to gracefully
// handle pathalogical cases and support multi-threaded programs. malloc
// by default also wastes a lot of memory, since it needs to support the
// allocation of long double (16 bytes) and memory leak detection. so if
// your your program doesn't need any of those things, then you can gain
// many benefits by using this allocator with the example settings above
//
// while this allocator has poor time complexity, it conforms rigorously
// to all the same api contracts as the standard malloc() implementation
// such as the behavior of null or zero parameters, roundups and copying

#ifndef TINYMALLOC_MAX_BYTES
#define TINYMALLOC_MAX_BYTES 1073741824
#endif
#ifndef TINYMALLOC_ALIGN
#define TINYMALLOC_ALIGN sizeof(max_align_t)
#endif
#ifndef TINYMALLOC_MAX_ALIGN
#define TINYMALLOC_MAX_ALIGN 4096
#endif
#ifndef TINYMALLOC_SIZE_T
#define TINYMALLOC_SIZE_T size_t
#endif
#ifndef TINYMALLOC_SET_ERRNO
#define TINYMALLOC_SET_ERRNO(x) errno = x;
#endif

#define TINYMALLOC_TMAX(T) \
  (((T) ~(T)0) > 1 ? (T) ~(T)0 : (T)(((uintmax_t)1 << (sizeof(T) * 8 - 1)) - 1))

static_assert(TINYMALLOC_MAX_BYTES <= TINYMALLOC_TMAX(TINYMALLOC_SIZE_T));
static_assert(TINYMALLOC_ALIGN >= sizeof(TINYMALLOC_SIZE_T));
static_assert(TINYMALLOC_ALIGN <= TINYMALLOC_MAX_ALIGN);

#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wanalyzer-malloc-leak"
#pragma GCC diagnostic ignored "-Wanalyzer-use-after-free"

alignas(TINYMALLOC_MAX_ALIGN) static struct {
  char memory[TINYMALLOC_MAX_BYTES];
  TINYMALLOC_SIZE_T used, last, free;
} heap;

static inline int isheap(void *mem) {
  return (uintptr_t)mem - (uintptr_t)heap.memory < (size_t)heap.used;
}

void *malloc(size_t need) {
  return memalign(TINYMALLOC_ALIGN, need);
}

void *calloc(size_t count, size_t size) {
  char *res;
  size_t used, need;
  used = heap.used;
  if (ckd_mul(&need, count, size))
    need = -1;
  if ((res = (char *)malloc(need)))
    if ((uintptr_t)res - (uintptr_t)heap.memory < (size_t)used)
      if (need)
        memset(res, 0, need);
  return res;
}

void free(void *ptr) {
  char *mem;
  if ((mem = (char *)ptr)) {
    unassert(isheap(mem));
    *(TINYMALLOC_SIZE_T *)mem = heap.free;
    heap.free = mem - heap.memory;
  }
}

size_t malloc_usable_size(void *ptr) {
  char *mem;
  if (!(mem = (char *)ptr))
    return 0;
  unassert(isheap(mem));
  return ((TINYMALLOC_SIZE_T *)mem)[-1];
}

void *memalign(size_t align, size_t need) {
  char *res;
  TINYMALLOC_SIZE_T next, next2, base, toto, *link, *link2;

  // normalize align
  if (align < sizeof(TINYMALLOC_SIZE_T))
    align = sizeof(TINYMALLOC_SIZE_T);
  while (align & (align - 1))
    ++align;
  if (align > TINYMALLOC_MAX_ALIGN)
    goto InvalidArgument;

  // normalize size
  if (!need)
    ++need;
  if (ckd_add(&need, need, sizeof(TINYMALLOC_SIZE_T) - 1))
    goto OutOfMemory;
  need &= -sizeof(TINYMALLOC_SIZE_T);

  // allocate from free list
  next = heap.free;
  link = &heap.free;
  while (next) {
    next2 = *(TINYMALLOC_SIZE_T *)(heap.memory + next);
    link2 = (TINYMALLOC_SIZE_T *)(heap.memory + next);
    if (need <= ((TINYMALLOC_SIZE_T *)(heap.memory + next))[-1]) {
      *link = next2;
      return (void *)(heap.memory + next);
    }
    next = next2;
    link = link2;
  }

  // allocate new static memory
  base = heap.used;
  base += sizeof(TINYMALLOC_SIZE_T);
  base += align - 1;
  base &= -align;
  if (ckd_add(&toto, base, need))
    goto OutOfMemory;
  if (toto > TINYMALLOC_MAX_BYTES)
    goto OutOfMemory;
  res = heap.memory + base;
  ((TINYMALLOC_SIZE_T *)res)[-1] = need;
  heap.used = toto;
  heap.last = base;
  return res;

  // we require more vespene gas
OutOfMemory:
  TINYMALLOC_SET_ERRNO(ENOMEM)
  return 0;
InvalidArgument:
  TINYMALLOC_SET_ERRNO(EINVAL)
  return 0;
}

void *realloc(void *ptr, size_t need) {
  char *res, *mem;
  TINYMALLOC_SIZE_T base, have, toto;
  if (!ptr) {
    res = (char *)malloc(need);
  } else {
    mem = (char *)ptr;
    unassert(isheap(mem));
    have = ((TINYMALLOC_SIZE_T *)mem)[-1];
    base = mem - heap.memory;
    if (need <= have) {
      res = mem;
    } else if (base == heap.last) {
      if (!need)
        ++need;
      if (ckd_add(&need, need, sizeof(TINYMALLOC_SIZE_T) - 1))
        goto OutOfMemory;
      need &= -sizeof(TINYMALLOC_SIZE_T);
      if (ckd_add(&toto, base, need))
        goto OutOfMemory;
      if (toto > TINYMALLOC_MAX_BYTES)
        goto OutOfMemory;
      ((TINYMALLOC_SIZE_T *)mem)[-1] = need;
      heap.used = toto;
      res = mem;
    } else if ((res = (char *)malloc(need))) {
      memcpy(res, mem, have);
      free(mem);
    }
  }
  return res;
OutOfMemory:
  TINYMALLOC_SET_ERRNO(ENOMEM)
  return 0;
}

void *realloc_in_place(void *ptr, size_t need) {
  if (!ptr)
    return 0;
  unassert(isheap(ptr));
  if (need <= ((TINYMALLOC_SIZE_T *)ptr)[-1])
    return ptr;
  return 0;
}

struct mallinfo mallinfo(void) {
  struct mallinfo mi = {.arena = heap.used};
  for (TINYMALLOC_SIZE_T chunk = heap.free; chunk;
       chunk = *(TINYMALLOC_SIZE_T *)(heap.memory + chunk)) {
    mi.ordblks += 1;
    mi.fordblks += ((TINYMALLOC_SIZE_T *)(heap.memory + chunk))[-1];
  }
  return mi;
}

int malloc_trim(size_t pad) {
  // tinymalloc doesn't support trimming
  return 0;
}

int mallopt(int param, int value) {
  // tinymalloc doesn't support runtime configuration
  return 0;
}

#pragma GCC pop_options
