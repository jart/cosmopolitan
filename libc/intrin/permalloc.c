// Copyright 2025 Justine Alexandra Roberts Tunney
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

#include "libc/cosmo.h"
#include "libc/intrin/maps.h"
#include "libc/runtime/runtime.h"

/**
 * Allocates dynamic memory that can't be freed or leak detected.
 *
 * This allocator only guarantees an 8 byte alignment. Small allocations
 * won't have `RLIMIT_AS` enforced on certain platforms and they are not
 * be accounted in the reported memory size. This allocator is leaky too
 * because it's implemented without locks. The algorithm goes slower for
 * multi-threaded applications, because it relies on a single atomic but
 * this allocator has the advantage of being snappier if the allocations
 * are being made during program startup. If your requested size is more
 * than the page size, then this function simply delegates it to mmap().
 *
 * @param new memory address, or null w/ errno
 */
void *cosmo_permalloc(size_t size) {

  // use mmap's internal allocator for small matters
  if (size < __pagesize) {
    if (!size)
      ++size;
    size += 7;
    size &= -8;
    return __maps_balloc(size);
  }

  // use mmap itself for large allocations
  return _mapanon(size);
}
