/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "third_party/nsync/mu_semaphore.h"
#include "third_party/nsync/time.h"
// clang-format off

#define ASSERT(x) _npassert(x)

struct sem {
	int64_t id;
};

static nsync_semaphore *sem_big_enough_for_sem = (nsync_semaphore *) (uintptr_t)(1 /
	(sizeof (struct sem) <= sizeof (*sem_big_enough_for_sem)));

/* Initialize *s; the initial value is 0. */
void nsync_mu_semaphore_init_sem (nsync_semaphore *s) {
	int newfd;
	struct sem *f = (struct sem *) s;
	f->id = 0;
	ASSERT (!sys_sem_init (0, &f->id));
	STRACE ("sem_init(0, [%ld]) → 0", f->id);
	ASSERT ((newfd = __sys_fcntl (f->id, F_DUPFD_CLOEXEC, 100)) != -1);
	ASSERT (!sys_sem_destroy (f->id));
	f->id = newfd;
}

/* Wait until the count of *s exceeds 0, and decrement it. */
errno_t nsync_mu_semaphore_p_sem (nsync_semaphore *s) {
	int e, rc;
	errno_t result;
	struct sem *f = (struct sem *) s;
	e = errno;
	BEGIN_CANCELLATION_POINT;
	rc = sys_sem_wait (f->id);
	END_CANCELLATION_POINT;
	STRACE ("sem_wait(%ld) → %d% m", f->id, rc);
	if (!rc) {
		result = 0;
	} else {
		result = errno;
		errno = e;
		ASSERT (result == ECANCELED);
	}
	return result;
}

/* Wait until one of:
   the count of *s is non-zero, in which case decrement *s and return 0;
   or abs_deadline expires, in which case return ETIMEDOUT. */
errno_t nsync_mu_semaphore_p_with_deadline_sem (nsync_semaphore *s, nsync_time abs_deadline) {
	int e, rc;
	errno_t result;
	struct sem *f = (struct sem *) s;
	e = errno;
	BEGIN_CANCELLATION_POINT;
	rc = sys_sem_timedwait (f->id, &abs_deadline);
	END_CANCELLATION_POINT;
	STRACE ("sem_timedwait(%ld, %s) → %d% m", f->id,
		DescribeTimespec(0, &abs_deadline), rc);
	if (!rc) {
		result = 0;
	} else {
		result = errno;
		errno = e;
		ASSERT (result == ETIMEDOUT ||
			result == ECANCELED);
	}
	return result;
}

/* Ensure that the count of *s is at least 1. */
void nsync_mu_semaphore_v_sem (nsync_semaphore *s) {
	struct sem *f = (struct sem *) s;
	ASSERT (!sys_sem_post (f->id));
	STRACE ("sem_post(%ld) → 0% m", f->id);
}
