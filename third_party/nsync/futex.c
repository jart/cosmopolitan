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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/time.h"
// clang-format off

#define FUTEX_WAIT_BITS_ FUTEX_BITSET_MATCH_ANY

errno_t _futex (atomic_int *, int, int, const struct timespec *, int *, int);
errno_t _futex_wake (atomic_int *, int, int) asm ("_futex");
int sys_futex_cp (atomic_int *, int, int, const struct timespec *, int *, int);

static int FUTEX_WAIT_;
static int FUTEX_PRIVATE_FLAG_;
static bool futex_is_supported;
static bool futex_timeout_is_relative;

__attribute__((__constructor__)) static void nsync_futex_init_ (void) {
	int e;
	atomic_int x;

	FUTEX_WAIT_ = FUTEX_WAIT;

	if (IsWindows ()) {
		futex_is_supported = true;
		return;
	}

	if (IsFreebsd ()) {
		futex_is_supported = true;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		return;
	}

        if (!(futex_is_supported = IsLinux () || IsOpenbsd ())) {
		return;
	}

	// In our testing, we found that the monotonic clock on various
	// popular systems (such as Linux, and some BSD variants) was no
	// better behaved than the realtime clock, and routinely took
	// large steps backwards, especially on multiprocessors. Given
	// that "monotonic" doesn't seem to mean what it says,
	// implementers of nsync_time might consider retaining the
	// simplicity of a single epoch within an address space, by
	// configuring any time synchronization mechanism (like ntp) to
	// adjust for leap seconds by adjusting the rate, rather than
	// with a backwards step.
	e = errno;
	atomic_store_explicit (&x, 0, memory_order_relaxed);
	if (IsLinux () &&
	    _futex (&x, FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME,
		    1, 0, 0, FUTEX_BITSET_MATCH_ANY) == -EAGAIN) {
		FUTEX_WAIT_ = FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
	} else if (!IsTiny () && IsLinux () &&
		   _futex (&x, FUTEX_WAIT_BITSET, 1, 0, 0,
			   FUTEX_BITSET_MATCH_ANY) == -EAGAIN) {
		FUTEX_WAIT_ = FUTEX_WAIT_BITSET;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
	} else if (IsOpenbsd () ||
		   (!IsTiny () && IsLinux () &&
		    !_futex_wake (&x, FUTEX_WAKE_PRIVATE, 1))) {
		FUTEX_WAIT_ = FUTEX_WAIT;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		futex_timeout_is_relative = true;
	} else {
		FUTEX_WAIT_ = FUTEX_WAIT;
		futex_timeout_is_relative = true;
	}
	errno = e;
}

static int nsync_futex_polyfill_ (atomic_int *w, int expect, struct timespec *abstime) {
	int rc;
	int64_t nanos, maxnanos;
	struct timespec now, wait, remain, deadline;

	if (!abstime) {
		deadline = timespec_max;
	} else {
		deadline = *abstime;
	}

	nanos = 100;
	maxnanos = __SIG_POLLING_INTERVAL_MS * 1000L * 1000;
	for (;;) {
		if (atomic_load_explicit (w, memory_order_acquire) != expect) {
			return 0;
		}
		now = timespec_real ();
		if (atomic_load_explicit (w, memory_order_acquire) != expect) {
			return 0;
		}
		if (timespec_cmp (now, deadline) >= 0) {
			break;
		}
		wait = timespec_fromnanos (nanos);
		remain = timespec_sub (deadline, now);
		if (timespec_cmp(wait, remain) > 0) {
			wait = remain;
		}
		if ((rc = clock_nanosleep (CLOCK_REALTIME, 0, &wait, 0))) {
			return -rc;
		}
		if (nanos < maxnanos) {
			nanos <<= 1;
			if (nanos > maxnanos) {
				nanos = maxnanos;
			}
		}
	}

	return -ETIMEDOUT;
}

static int nsync_futex_wait_win32_ (atomic_int *w, int expect, char pshare, struct timespec *timeout) {
	int rc;
	uint32_t ms;
	struct timespec deadline, interval, remain, wait, now;

	if (timeout) {
		deadline = *timeout;
	} else {
		deadline = timespec_max;
	}

	while (!(rc = _check_interrupts (false, 0))) {
		now = timespec_real ();
		if (timespec_cmp (now, deadline) > 0) {
			rc = etimedout();
			break;
		}
		remain = timespec_sub (deadline, now);
		interval = timespec_frommillis (__SIG_POLLING_INTERVAL_MS);
		wait = timespec_cmp (remain, interval) > 0 ? interval : remain;
		if (atomic_load_explicit (w, memory_order_acquire) != expect) {
			break;
		}
		if (WaitOnAddress (w, &expect, sizeof(int), timespec_tomillis (wait))) {
			break;
		} else {
			ASSERT (GetLastError () == ETIMEDOUT);
		}
	}

	return rc;
}

static struct timespec *nsync_futex_timeout_ (struct timespec *memory,
					      const struct timespec *abstime) {
	struct timespec now;
	if (!abstime) {
		return 0;
	} else if (!futex_timeout_is_relative) {
		*memory = *abstime;
		return memory;
	} else {
		now = timespec_real ();
		if (timespec_cmp (now, *abstime) > 0) {
			*memory = (struct timespec){0};
		} else {
			*memory = timespec_sub (*abstime, now);
		}
		return memory;
	}
}

int nsync_futex_wait_ (atomic_int *w, int expect, char pshare, struct timespec *abstime) {
	int e, rc, op, fop;
	struct PosixThread *pt = 0;
	struct timespec tsmem, *timeout;

	op = FUTEX_WAIT_;
	if (pshare == PTHREAD_PROCESS_PRIVATE) {
		op |= FUTEX_PRIVATE_FLAG_;
	}

	if (abstime && timespec_cmp (*abstime, timespec_zero) <= 0) {
		rc = -ETIMEDOUT;
		goto Finished;
	}

	if (atomic_load_explicit (w, memory_order_acquire) != expect) {
		rc = -EAGAIN;
		goto Finished;
	}

	timeout = nsync_futex_timeout_ (&tsmem, abstime);

	LOCKTRACE ("futex(%t [%d], %s, %#x, %s) → ...",
		   w, atomic_load_explicit (w, memory_order_relaxed),
		   DescribeFutexOp (op), expect,
		   DescribeTimespec (0, timeout));

	if (futex_is_supported) {
		e = errno;
		if (IsWindows ()) {
			// Windows 8 futexes don't support multiple processes :(
			if (pshare) goto Polyfill;
			rc = nsync_futex_wait_win32_ (w, expect, pshare, timeout);
		} else if (IsFreebsd ()) {
			rc = sys_umtx_timedwait_uint (w, expect, pshare, timeout);
		} else {
			if (IsOpenbsd()) {
				// OpenBSD 6.8 futex() returns errors as
				// positive numbers, without setting CF.
				// This irregularity is fixed in 7.2 but
				// unfortunately OpenBSD futex() defines
				// its own ECANCELED condition, and that
				// overlaps with our system call wrapper
				if ((pt = (struct PosixThread *)__get_tls()->tib_pthread)) {
					pt->flags &= ~PT_OPENBSD_KLUDGE;
				}
			}
			rc = sys_futex_cp (w, op, expect, timeout, 0, FUTEX_WAIT_BITS_);
			if (IsOpenbsd()) {
				// Handle the OpenBSD 6.x irregularity.
				if (rc > 0) {
					errno = rc;
					rc = -1;
				}
				// Check if ECANCELED came from the kernel
				// because a SA_RESTART signal handler was
				// invoked, such as our SIGTHR callback.
				if (rc == -1 && errno == ECANCELED &&
				    pt && (~pt->flags & PT_OPENBSD_KLUDGE)) {
					errno = EINTR;
				}
			}
		}
		if (rc == -1) {
			rc = -errno;
			errno = e;
		}
	} else {
	Polyfill:
		__get_tls()->tib_flags |= TIB_FLAG_TIME_CRITICAL;
		rc = nsync_futex_polyfill_ (w, expect, timeout);
		__get_tls()->tib_flags &= ~TIB_FLAG_TIME_CRITICAL;
	}

Finished:
	STRACE ("futex(%t [%d], %s, %#x, %s) → %s",
		w, atomic_load_explicit (w, memory_order_relaxed),
		DescribeFutexOp (op), expect,
		DescribeTimespec (0, abstime),
		DescribeErrno (rc));

	return rc;
}

int nsync_futex_wake_ (atomic_int *w, int count, char pshare) {
	int e, rc, op, fop;

	ASSERT (count == 1 || count == INT_MAX);

	op = FUTEX_WAKE;
	if (pshare == PTHREAD_PROCESS_PRIVATE) {
		op |= FUTEX_PRIVATE_FLAG_;
	}

	if (futex_is_supported) {
		if (IsWindows ()) {
			if (pshare) {
				goto Polyfill;
			}
			if (count == 1) {
				WakeByAddressSingle (w);
			} else {
				WakeByAddressAll (w);
			}
			rc = 0;
		} else if (IsFreebsd ()) {
			if (pshare) {
				fop = UMTX_OP_WAKE;
			} else {
				fop = UMTX_OP_WAKE_PRIVATE;
			}
			rc = _futex_wake (w, fop, count);
		} else {
			rc = _futex_wake (w, op, count);
		}
	} else {
	Polyfill:
		sched_yield ();
		rc = 0;
	}

	STRACE ("futex(%t [%d], %s, %d) → %s",
		w, atomic_load_explicit (w, memory_order_relaxed),
		DescribeFutexOp (op), count, DescribeErrno (rc));

	return rc;
}
