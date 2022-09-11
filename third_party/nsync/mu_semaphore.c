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
#include "libc/dce.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/mu_semaphore.internal.h"
// clang-format off

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init (nsync_semaphore *s) {
	if (!IsWindows ())
		nsync_mu_semaphore_init_futex (s);
	else
		nsync_mu_semaphore_init_win32 (s);
}

/* Wait until the count of *s exceeds 0, and decrement it. */
void nsync_mu_semaphore_p (nsync_semaphore *s) {
	if (!IsWindows ())
		nsync_mu_semaphore_p_futex (s);
	else
		nsync_mu_semaphore_p_win32 (s);
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
int nsync_mu_semaphore_p_with_deadline (nsync_semaphore *s, nsync_time abs_deadline) {
	if (!IsWindows ())
		return nsync_mu_semaphore_p_with_deadline_futex (s, abs_deadline);
	else
		return nsync_mu_semaphore_p_with_deadline_win32 (s, abs_deadline);
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v (nsync_semaphore *s) {
	if (!IsWindows ())
		nsync_mu_semaphore_v_futex (s);
	else
		nsync_mu_semaphore_v_win32 (s);
}
