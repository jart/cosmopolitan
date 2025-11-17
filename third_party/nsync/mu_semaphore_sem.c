/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/time.h"
#include "third_party/nsync/mu_semaphore.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "third_party/nsync/time.h"

/**
 * @fileoverview Semaphores w/ POSIX Semaphores API.
 */

struct sem {
	int64_t id;
};

static nsync_semaphore *sem_big_enough_for_sem = (nsync_semaphore *) (uintptr_t)(1 /
	(sizeof (struct sem) <= sizeof (*sem_big_enough_for_sem)));

/* Initialize *s; the initial value is 0. */
bool nsync_mu_semaphore_init_sem (nsync_semaphore *s) {
	struct sem *f = (struct sem *) s;
	int e = errno;
	int64_t id = 0;
	int rc = sys_sem_init (0, &id);
	STRACE ("sem_init(0, [%ld]) → %d", id, rc);
	if (rc != 0) {
		errno = e;
		return false;
	}
	if (id >= 50) {
		f->id = id;
		return true;
	}
	int lol = __sys_fcntl (id, 12 /* F_DUPFD_CLOEXEC */, 50);
	STRACE ("fcntl(%ld, F_DUPFD_CLOEXEC, 50) → %d", id, lol);
	rc = sys_close (id);
	STRACE ("close(%ld) → %d", id, rc);
	if (lol == -1) {
		errno = e;
		return false;
	}
	f->id = lol;
	errno = e;
	return true;
}

/* Destroys *s. */
void nsync_mu_semaphore_destroy_sem (nsync_semaphore *s) {
	/* TODO: Why is it possible for this to fail with EBADF? */
	struct sem *f = (struct sem *) s;
	int e = errno;
	sys_close (f->id);
	f->id = -1;
	errno = e;
}

/* Wait until the count of *s exceeds 0, and decrement it. If POSIX cancellations
   are currently disabled by the thread, then this function always succeeds. When
   they're enabled in MASKED mode, this function may return ECANCELED. Otherwise,
   cancellation will occur by unwinding cleanup handlers pushed to the stack. */
errno_t nsync_mu_semaphore_p_sem (nsync_semaphore *s) {
	int e, rc;
	errno_t result;
	struct sem *f = (struct sem *) s;
	e = errno;
	rc = sys_sem_wait (f->id);
	STRACE ("sem_wait(%ld) → %d% m", f->id, rc);
	if (!rc) {
		result = 0;
	} else {
		result = errno;
		errno = e;
		unassert (result == ECANCELED);
	}
	return result;
}

/* Like nsync_mu_semaphore_p() this waits for the count of *s to exceed 0,
   while additionally supporting a time parameter specifying at what point
   in the future ETIMEDOUT should be returned, if neither cancellation, or
   semaphore release happens. */
errno_t nsync_mu_semaphore_p_with_deadline_sem (nsync_semaphore *s, int clock,
						nsync_time abs_deadline) {
	int e, rc;
	errno_t result;
	struct sem *f = (struct sem *) s;

	// convert monotonic back to realtime just for netbsd
	if (clock && nsync_time_cmp (abs_deadline, nsync_time_no_deadline)) {
		struct timespec now, delta;
		if (clock_gettime (clock, &now))
			return EINVAL;
		delta = timespec_subz (abs_deadline, now);
		clock_gettime (CLOCK_REALTIME, &now);
		abs_deadline = timespec_add (now, delta);
	}

	e = errno;
	rc = sys_sem_timedwait (f->id, &abs_deadline);
	STRACE ("sem_timedwait(%ld, %s) → %d% m", f->id,
		DescribeTimespec(0, &abs_deadline), rc);
	if (!rc) {
		result = 0;
	} else {
		result = errno;
		errno = e;
		unassert (result == ETIMEDOUT ||
			  result == ECANCELED);
	}
	return result;
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_sem (nsync_semaphore *s) {
	struct sem *f = (struct sem *) s;
	unassert (!sys_sem_post (f->id));
	STRACE ("sem_post(%ld) → 0% m", f->id);
}
