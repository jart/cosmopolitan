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
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
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
static bool FUTEX_IS_SUPPORTED;
bool FUTEX_TIMEOUT_IS_ABSOLUTE;

__attribute__((__constructor__)) static void nsync_futex_init_ (void) {
	atomic_int x;

	FUTEX_WAIT_ = FUTEX_WAIT;

	if (IsWindows ()) {
		FUTEX_IS_SUPPORTED = true;
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
		return;
	}

	if (IsFreebsd ()) {
		FUTEX_IS_SUPPORTED = true;
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		return;
	}

        if (!(FUTEX_IS_SUPPORTED = IsLinux () || IsOpenbsd ())) {
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
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
	atomic_store_explicit (&x, 0, memory_order_relaxed);
	if (IsLinux () &&
	    _futex (&x, FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME,
		    1, 0, 0, FUTEX_BITSET_MATCH_ANY) == -EAGAIN) {
		FUTEX_WAIT_ = FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
	} else if (!IsTiny () && IsLinux () &&
		   _futex (&x, FUTEX_WAIT_BITSET, 1, 0, 0,
			   FUTEX_BITSET_MATCH_ANY) == -EAGAIN) {
		FUTEX_WAIT_ = FUTEX_WAIT_BITSET;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
	} else if (IsOpenbsd () ||
		   (!IsTiny () && IsLinux () &&
		    !_futex_wake (&x, FUTEX_WAKE_PRIVATE, 1))) {
		FUTEX_WAIT_ = FUTEX_WAIT;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
	} else {
		FUTEX_WAIT_ = FUTEX_WAIT;
	}
}

static int nsync_futex_polyfill_ (atomic_int *w, int expect, struct timespec *timeout) {
	int rc;
	int64_t nanos, maxnanos;
	struct timespec ts, deadline;

	ts = nsync_time_now ();
	if (!timeout) {
		deadline = nsync_time_no_deadline;
	} else if (FUTEX_TIMEOUT_IS_ABSOLUTE) {
		deadline = *timeout;
	} else {
		deadline = nsync_time_add (ts, *timeout);
	}

	nanos = 100;
	maxnanos = __SIG_POLLING_INTERVAL_MS * 1000L * 1000;
	while (nsync_time_cmp (deadline, ts) > 0) {
		ts = nsync_time_add (ts, _timespec_fromnanos (nanos));
		if (nsync_time_cmp (ts, deadline) > 0) {
			ts = deadline;
		}
		if (atomic_load_explicit (w, memory_order_acquire) != expect) {
			return 0;
		}
		if ((rc = nsync_time_sleep_until (ts))) {
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
		deadline = nsync_time_no_deadline;
	}

	while (!(rc = _check_interrupts (false, 0))) {
		now = nsync_time_now ();
		if (nsync_time_cmp (now, deadline) > 0) {
			rc = etimedout();
			break;
		}
		remain = nsync_time_sub (deadline, now);
		interval = _timespec_frommillis (__SIG_POLLING_INTERVAL_MS);
		wait = nsync_time_cmp (remain, interval) > 0 ? interval : remain;
		if (atomic_load_explicit (w, memory_order_acquire) != expect) {
			break;
		}
		if (WaitOnAddress (w, &expect, sizeof(int), _timespec_tomillis (wait))) {
			break;
		} else {
			ASSERT (GetLastError () == ETIMEDOUT);
		}
	}

	return rc;
}

int nsync_futex_wait_ (atomic_int *w, int expect, char pshare, struct timespec *timeout) {
	int e, rc, op, fop;

	if (atomic_load_explicit (w, memory_order_acquire) != expect) {
		return -EAGAIN;
	}

	op = FUTEX_WAIT_;
	if (pshare == PTHREAD_PROCESS_PRIVATE) {
		op |= FUTEX_PRIVATE_FLAG_;
	}

	LOCKTRACE ("futex(%t [%d], %s, %#x, %s) → ...",
		   w, atomic_load_explicit (w, memory_order_relaxed),
		   DescribeFutexOp (op), expect,
		   DescribeTimespec (0, timeout));

	if (FUTEX_IS_SUPPORTED) {
		e = errno;
		if (IsWindows ()) {
			// Windows 8 futexes don't support multiple processes :(
			if (pshare) goto Polyfill;
			rc = nsync_futex_wait_win32_ (w, expect, pshare, timeout);
		} else if (IsFreebsd ()) {
			rc = sys_umtx_timedwait_uint (w, expect, pshare, timeout);
		} else {
			rc = sys_futex_cp (w, op, expect, timeout, 0, FUTEX_WAIT_BITS_);
			if (IsOpenbsd() && rc > 0) {
				// OpenBSD futex() returns errors as
				// positive numbers without setting the
				// carry flag. It's an irregular miracle
				// because OpenBSD returns ECANCELED if
				// futex() is interrupted w/ SA_RESTART
				// so we're able to tell it apart from
				// PT_MASKED which causes the wrapper
				// to put ECANCELED into errno.
				if (rc == ECANCELED) {
					rc = EINTR;
				}
				errno = rc;
				rc = -1;
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

	STRACE ("futex(%t [%d], %s, %#x, %s) → %s",
		w, atomic_load_explicit (w, memory_order_relaxed),
		DescribeFutexOp (op), expect,
		DescribeTimespec (0, timeout),
		DescribeErrnoResult (rc));

	return rc;
}

int nsync_futex_wake_ (atomic_int *w, int count, char pshare) {
	int e, rc, op, fop;

	ASSERT (count == 1 || count == INT_MAX);

	op = FUTEX_WAKE;
	if (pshare == PTHREAD_PROCESS_PRIVATE) {
		op |= FUTEX_PRIVATE_FLAG_;
	}

	if (FUTEX_IS_SUPPORTED) {
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
		DescribeFutexOp(op), count, DescribeErrnoResult(rc));

	return rc;
}
