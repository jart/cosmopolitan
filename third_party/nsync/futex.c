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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/timer.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/time.h"
// clang-format off

/* futex() polyfill w/ sched_yield() fallback */

#define FUTEX_WAIT_BITS_ FUTEX_BITSET_MATCH_ANY

int _futex (int *, int, int, const struct timespec *, int *, int);

static int FUTEX_WAIT_;
static int FUTEX_PRIVATE_FLAG_;
static bool FUTEX_IS_SUPPORTED;
bool FUTEX_TIMEOUT_IS_ABSOLUTE;

__attribute__((__constructor__)) static void nsync_futex_init_ (void) {
	int x = 0;

	FUTEX_WAIT_ = FUTEX_WAIT;

	if (IsWindows ()) {
		FUTEX_IS_SUPPORTED = true;
		return;
	}

	if (IsFreebsd ()) {
		FUTEX_IS_SUPPORTED = true;
		FUTEX_TIMEOUT_IS_ABSOLUTE = true;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		return;
	}

        if (!(FUTEX_IS_SUPPORTED = IsLinux () || IsOpenbsd ())) {
		// we're using sched_yield() so let's
		// avoid needless clock_gettime calls
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
		    !_futex (&x, FUTEX_WAKE_PRIVATE, 1, 0, 0, 0))) {
		FUTEX_WAIT_ = FUTEX_WAIT;
		FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
	} else {
		FUTEX_WAIT_ = FUTEX_WAIT;
	}
}

static int nsync_futex_polyfill_ (int *p, int expect, struct timespec *timeout) {
	int64_t nanos, maxnanos;
	nsync_atomic_uint32_ *w;
	struct timespec ts, deadline;

	w = (nsync_atomic_uint32_ *)p;
	if (ATM_LOAD (p) != expect) {
		return -EAGAIN;
	}

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
		if (ATM_LOAD (p) != expect) {
			return 0;
		}
		ts = nsync_time_add (ts, _timespec_fromnanos (nanos));
		if (nsync_time_cmp (ts, deadline) > 0) {
			ts = deadline;
		}
		if (nsync_time_sleep_until (ts)) {
			return -EINTR;
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

int nsync_futex_wait_ (int *p, int expect, char pshare, struct timespec *timeout) {
	uint32_t ms;
	int rc, op, fop;

	op = FUTEX_WAIT_;
	if (pshare == PTHREAD_PROCESS_PRIVATE) {
		op |= FUTEX_PRIVATE_FLAG_;
	}

	if (FUTEX_IS_SUPPORTED) {
		if (IsWindows ()) {
			// Windows 8 futexes don't support multiple processes :(
			if (pshare) {
				goto Polyfill;
			}
			if (_check_interrupts (false, 0)) {
				rc = -EINTR;
			} else {
				if (timeout) {
					ms = _timespec_tomillis (*timeout);
				} else {
					ms = -1;
				}
				if (WaitOnAddress (p, &expect, sizeof(int), ms)) {
					rc = 0;
				} else {
					rc = -GetLastError ();
				}
			}
		} else if (IsFreebsd ()) {
			rc = sys_umtx_timedwait_uint (
				p, expect, pshare, timeout);
		} else {
			rc = _futex (p, op, expect, timeout, 0,
				     FUTEX_WAIT_BITS_);
			if (IsOpenbsd() && rc > 0) {
				rc = -rc;
			}
		}
	} else {
	Polyfill:
		__get_tls()->tib_flags |= TIB_FLAG_TIME_CRITICAL;
		rc = nsync_futex_polyfill_ (p, expect, timeout);
		__get_tls()->tib_flags &= ~TIB_FLAG_TIME_CRITICAL;
	}

	STRACE ("futex(%t, %s, %#x, %s) → %s",
		p, DescribeFutexOp (op), expect,
		DescribeTimespec (0, timeout),
		DescribeErrnoResult (rc));

	return rc;
}

int nsync_futex_wake_ (int *p, int count, char pshare) {
	int e, rc, op, fop;
	int wake (void *, int, int) asm ("_futex");

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
				WakeByAddressSingle (p);
			} else {
				WakeByAddressAll (p);
			}
			rc = 0;
		} else if (IsFreebsd ()) {
			if (pshare) {
				fop = UMTX_OP_WAKE;
			} else {
				fop = UMTX_OP_WAKE_PRIVATE;
			}
			rc = sys_umtx_op (p, fop, count, 0, 0);
		} else {
			rc = wake (p, op, count);
		}
	} else {
	Polyfill:
		sched_yield ();
		rc = 0;
	}

	STRACE ("futex(%t, %s, %d) → %s",
		p, DescribeFutexOp(op),
		count, DescribeErrnoResult(rc));

	return rc;
}
