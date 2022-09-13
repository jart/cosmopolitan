/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/extend.internal.h"
#include "libc/macros.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/malloc.internal.h"
// clang-format off

static char *nsync_malloc_endptr_;
static size_t nsync_malloc_total_;
static atomic_char nsync_malloc_lock_;

/* nsync_malloc_() is a malloc-like routine used by mutex and condition
   variable code to allocate waiter structs. This allows *NSYNC mutexes
   to be used by malloc(), by providing another, simpler allocator here.
   The intent is that the implicit NULL value here can be overridden by
   a client declaration that uses an initializer. */
void *nsync_malloc_ (size_t size) {
	char *start;
	size_t offset;
	size = ROUNDUP (size, __BIGGEST_ALIGNMENT__);
	while (atomic_exchange (&nsync_malloc_lock_, 1)) nsync_yield_ ();
	offset = nsync_malloc_total_;
	nsync_malloc_total_ += size;
	start = (char *) kMemtrackNsyncStart;
	if (!nsync_malloc_endptr_) nsync_malloc_endptr_ = start;
	nsync_malloc_endptr_ =
		_extend (start, nsync_malloc_total_, nsync_malloc_endptr_,
			 kMemtrackNsyncStart + kMemtrackNsyncSize);
	atomic_store_explicit (&nsync_malloc_lock_, 0, memory_order_relaxed);
	return start + offset;
}
