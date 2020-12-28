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
#include "libc/assert.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/fmt/bing.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/sysconf.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

/* TODO(jart): DELETE */

struct TestMemoryStack g_testmem;
struct TestMemoryStack g_testmem_trash;
static struct TestAllocation g_testmem_scratch[2][8];
static const char kMemZero[1];
static bool g_atstartofpage;

static struct TestAllocation testmem_push(struct TestMemoryStack *stack,
                                          struct TestAllocation entry) {
  if (stack->i == stack->n) {
    if (!__grow(&stack->p, &stack->n, sizeof(struct TestAllocation), 0)) {
      abort();
    }
  }
  return (stack->p[stack->i++] = entry);
}

static struct TestAllocation testmem_pop(struct TestMemoryStack *stack) {
  assert(stack->i > 0);
  struct TestAllocation res = stack->p[--stack->i];
  return res;
}

static void testmem_destroy(struct TestAllocation alloc) {
  if (munmap(alloc.mapaddr, alloc.mapsize) == -1) perror("munmap"), __die();
}

static struct TestAllocation talloc(size_t n) {
  struct TestAllocation alloc;
  if (n) {
    while (g_testmem_trash.i) {
      struct TestAllocation trash = testmem_pop(&g_testmem_trash);
      if (n <= trash.usersize) {
        return trash;
      } else {
        testmem_destroy(trash);
      }
    }
    alloc.mapsize = ROUNDUP(n + PAGESIZE * 2, FRAMESIZE);
    CHECK_NE(MAP_FAILED, (alloc.mapaddr = mapanon(alloc.mapsize)));
    CHECK_NE(-1, mprotect(alloc.mapaddr, PAGESIZE, PROT_NONE));
    CHECK_NE(-1, mprotect((char *)alloc.mapaddr + alloc.mapsize - PAGESIZE,
                          PAGESIZE, PROT_NONE));
    alloc.useraddr = (char *)alloc.mapaddr + PAGESIZE;
    alloc.usersize = alloc.mapsize - PAGESIZE * 2;
    CHECK_GE(alloc.usersize, n);
    return alloc;
  } else {
    alloc.mapaddr = (/*unconst*/ void *)kMemZero;
    alloc.mapsize = 0;
    alloc.useraddr = (/*unconst*/ void *)kMemZero;
    alloc.usersize = 0;
    return alloc;
  }
}

static textstartup void testmem_init(void) {
  g_testmem.p = g_testmem_scratch[0];
  g_testmem.n = ARRAYLEN(g_testmem_scratch[0]);
  g_testmem_trash.p = g_testmem_scratch[1];
  g_testmem_trash.n = ARRAYLEN(g_testmem_scratch[1]);
}

const void *const testmem_ctor[] initarray = {testmem_init};

FIXTURE(testmemory, triggerOffByOneArrayErrors) {
  /* automate testing buffer overflows *and* underflows */
  g_atstartofpage = true;
}

/**
 * Allocates memory with properties useful for testing.
 *
 * This returns a pointer 𝑝 where reading or writing to either 𝑝[-1] or
 * 𝑝[𝑛+𝟷] will immediately trigger a segmentation fault; and bytes are
 * initialized to 10100101 (A5).
 *
 * Implementation Details: Accomplishing this entails two things. First,
 * we grant each allocation a page granular memory mapping, with access
 * to the two adjacent pages disabled. Second, since hardware memory
 * protection isn't 1-byte granular, we add a fixture so each test runs
 * a second time; the first call we return a pointer where the data is
 * placed on the righthand side, and the second call we return the data
 * on the lefthand side, thereby allowing both underflow/overflow
 * off-by-one out-of-bounds accesses to be detected.
 */
void *tmalloc(size_t n) {
  struct TestAllocation alloc = talloc(n);
  memset(alloc.useraddr, 0xa5, alloc.usersize);
  testmem_push(&g_testmem, alloc);
  return (char *)alloc.useraddr + (g_atstartofpage ? 0 : alloc.usersize - n);
}

/**
 * Same as tmalloc() but guarantees a specific alignment.
 *
 * Reading or writing to either 𝑝[-1] or 𝑝[roundup(𝑛+𝟷,𝑎)] will
 * immediately trigger a segmentation fault.
 *
 * @param 𝑎 is alignment in bytes, e.g. 16
 * @param 𝑛 is number of bytes
 */
void *tmemalign(size_t a, size_t n) {
  /* TODO(jart): ASAN detect 𝑝[𝑛+𝟷] */
  return tmalloc(ROUNDUP(n, a));
}

/**
 * Same as tunbing() w/ alignment guarantee.
 */
void *tunbinga(size_t a, const char16_t *binglyphs) {
  size_t size;
  EXPECT_NE(0, (size = strlen16(binglyphs)));
  return unbingbuf(tmemalign(a, size), size, binglyphs, -1);
}

/**
 * Decodes CP437 glyphs to bounds-checked binary buffer, e.g.
 *
 *   char *mem = tunbing(u" ☺☻♥♦");
 *   EXPECT_EQ(0, memcmp("\0\1\2\3\4", mem, 5));
 *   tfree(mem);
 *
 * @see tunbing(), unbingstr(), unbing()
 */
void *tunbing(const char16_t *binglyphs) {
  return tunbinga(1, binglyphs);
}

/**
 * Frees memory allocated with tmalloc().
 * This needs to be called in LIFO order.
 * @param
 */
void(tfree)(void *p) {
  struct TestAllocation alloc;
  __tfree_check(p);
  alloc = testmem_pop(&g_testmem);
  if (alloc.mapsize) testmem_push(&g_testmem_trash, alloc);
}

char *tstrdup(const char *s) {
  return strcpy(tmalloc(strlen(s) + 1), s);
}
