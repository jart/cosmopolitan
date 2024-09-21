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
#include "libc/sysv/consts/futex.h"
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/ulock.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/runtime/clktck.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/time.h"
#include "third_party/nsync/common.internal.h"
#include "third_party/nsync/futex.internal.h"
#include "third_party/nsync/time.h"

#define FUTEX_WAIT_BITS_ FUTEX_BITSET_MATCH_ANY

errno_t _futex (atomic_int *, int, int, const struct timespec *, int *, int);
errno_t _futex_wake (atomic_int *, int, int) asm ("_futex");
int sys_futex_cp (atomic_int *, int, int, const struct timespec *, int *, int);

static struct NsyncFutex {
	atomic_uint once;
	int FUTEX_WAIT_;
	int FUTEX_PRIVATE_FLAG_;
	int FUTEX_CLOCK_REALTIME_;
	bool is_supported;
	bool timeout_is_relative;
} nsync_futex_;

static void nsync_futex_init_ (void) {
	int e;
	atomic_int x;

	nsync_futex_.FUTEX_WAIT_ = FUTEX_WAIT;

	if (IsWindows ()) {
		nsync_futex_.is_supported = true;
		return;
	}

	if (IsXnu ()) {
		nsync_futex_.is_supported = true;
		nsync_futex_.timeout_is_relative = true;
		return;
	}

	if (IsFreebsd ()) {
		nsync_futex_.is_supported = true;
		nsync_futex_.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		return;
	}

	if (!(nsync_futex_.is_supported = IsLinux () || IsOpenbsd ()))
		return;

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
		nsync_futex_.FUTEX_WAIT_ = FUTEX_WAIT_BITSET;
		nsync_futex_.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		nsync_futex_.FUTEX_CLOCK_REALTIME_ = FUTEX_CLOCK_REALTIME;
	} else if (IsOpenbsd () ||
		   (IsLinux () &&
		    !_futex_wake (&x, FUTEX_WAKE_PRIVATE, 1))) {
		nsync_futex_.FUTEX_WAIT_ = FUTEX_WAIT;
		nsync_futex_.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		nsync_futex_.timeout_is_relative = true;
	} else {
		nsync_futex_.FUTEX_WAIT_ = FUTEX_WAIT;
		nsync_futex_.timeout_is_relative = true;
	}
	errno = e;
}

static uint32_t nsync_time_64to32u (uint64_t duration) {
	if (duration <= -1u)
		return duration;
	return -1u;
}

static int nsync_futex_polyfill_ (atomic_int *w, int expect, int clock, struct timespec *abstime) {
	for (;;) {
		if (atomic_load_explicit (w, memory_order_acquire) != expect)
			return 0;
		if (_weaken (pthread_testcancel_np) &&
		    _weaken (pthread_testcancel_np) ())
			return -ECANCELED;
		struct timespec now;
		if (clock_gettime (clock, &now))
			return -EINVAL;
		if (abstime && timespec_cmp (now, *abstime) >= 0)
			return -ETIMEDOUT;
		pthread_yield_np ();
	}
}

static int nsync_futex_wait_win32_ (atomic_int *w, int expect, char pshare,
				    int clock, const struct timespec *timeout,
				    struct PosixThread *pt,
				    sigset_t waitmask) {
#ifdef __x86_64__
	int sig;
	bool32 ok;
	struct timespec deadline, wait, now;

	if (timeout) {
		deadline = *timeout;
	} else {
		deadline = timespec_max;
	}

	for (;;) {
		if (clock_gettime (clock, &now))
			return einval ();
		if (timespec_cmp (now, deadline) >= 0)
			return etimedout ();
		wait = timespec_sub (deadline, now);
		if (atomic_load_explicit (w, memory_order_acquire) != expect)
			return 0;
		if (pt) {
			if (_check_cancel () == -1)
				return -1; /* ECANCELED */
			if ((sig = __sig_get (waitmask))) {
				__sig_relay (sig, SI_KERNEL, waitmask);
				if (_check_cancel () == -1)
					return -1; /* ECANCELED */
				return eintr ();
			}
			pt->pt_blkmask = waitmask;
			atomic_store_explicit (&pt->pt_blocker, w, memory_order_release);
		}
		ok = WaitOnAddress (w, &expect, sizeof(int), nsync_time_64to32u (timespec_tomillis (wait)));
		if (pt) {
			/* __sig_wake wakes our futex without changing `w` after enqueing signals */
			atomic_store_explicit (&pt->pt_blocker, 0, memory_order_release);
			if (ok && atomic_load_explicit (w, memory_order_acquire) == expect && (sig = __sig_get (waitmask))) {
				__sig_relay (sig, SI_KERNEL, waitmask);
				if (_check_cancel () == -1)
					return -1; /* ECANCELED */
				return eintr ();
			}
		}
		if (ok) {
			return 0;
		} else {
			ASSERT (GetLastError () == ETIMEDOUT);
		}
	}
#else
	return 0;
#endif /* __x86_64__ */
}

static int nsync_futex_fix_timeout_ (struct timespec *memory, int clock,
				     const struct timespec *abstime,
				     struct timespec **result) {
	struct timespec now;
	if (!abstime) {
		*result = 0;
		return 0;
	} else if (!nsync_futex_.timeout_is_relative) {
		*memory = *abstime;
		*result = memory;
		return 0;
	} else {
		if (clock_gettime (clock, &now))
			return -EINVAL;
		*memory = timespec_subz (*abstime, now);
		*result = memory;
		return 0;
	}
}

int nsync_futex_wait_ (atomic_int *w, int expect, char pshare,
		       int clock, const struct timespec *abstime) {
	int e, rc, op;
	struct CosmoTib *tib;
	struct PosixThread *pt;
	struct timespec tsmem;
	struct timespec *timeout = 0;

	cosmo_once (&nsync_futex_.once, nsync_futex_init_);

	op = nsync_futex_.FUTEX_WAIT_;
	if (pshare == PTHREAD_PROCESS_PRIVATE)
		op |= nsync_futex_.FUTEX_PRIVATE_FLAG_;
	if (clock == CLOCK_REALTIME ||
	    clock == CLOCK_REALTIME_COARSE)
		op |= nsync_futex_.FUTEX_CLOCK_REALTIME_;

	if (abstime && timespec_cmp (*abstime, timespec_zero) <= 0) {
		rc = -ETIMEDOUT;
		goto Finished;
	}

	if (atomic_load_explicit (w, memory_order_acquire) != expect) {
		rc = -EAGAIN;
		goto Finished;
	}

	if ((rc = nsync_futex_fix_timeout_ (&tsmem, clock, abstime, &timeout)))
		goto Finished;

	LOCKTRACE ("futex(%t [%d], %s, %#x, %s) → ...",
		   w, atomic_load_explicit (w, memory_order_relaxed),
		   DescribeFutexOp (op), expect,
		   DescribeTimespec (0, timeout));

	tib = __get_tls();
	pt = (struct PosixThread *)tib->tib_pthread;

	if (nsync_futex_.is_supported) {
		e = errno;
		if (IsWindows ()) {
			// Windows 8 futexes don't support multiple processes :(
			if (pshare) goto Polyfill;
			sigset_t m = __sig_block ();
			rc = nsync_futex_wait_win32_ (w, expect, pshare, clock, timeout, pt, m);
			__sig_unblock (m);
		} else if (IsXnu ()) {

			/* XNU ulock (used by cosmo futexes) is an internal API, however:

			     1. Unlike GCD it's cancelable i.e. can be EINTR'd by signals
			     2. We have no choice but to use ulock for joining threads
			     3. Grand Central Dispatch requires a busy loop workaround
			     4. ulock makes our mutexes use 20% more system time (meh)
			     5. ulock makes our mutexes use 40% less wall time (good)
			     6. ulock makes our mutexes use 64% less user time (woop)
			     7. GCD uses Mach timestamps D: ulock just uses rel. time

			   ulock is an outstanding system call that must be used.
			   gcd is not an acceptable alternative to ulock. */

			uint32_t op, us;
			if (pshare) {
				op = UL_COMPARE_AND_WAIT_SHARED;
			} else {
				op = UL_COMPARE_AND_WAIT;
			}
			if (timeout) {
				us = nsync_time_64to32u (timespec_tomicros (*timeout));
			} else {
				us = -1u;
			}
			rc = ulock_wait (op, w, expect, us);
			if (rc > 0) rc = 0; // don't care about #waiters
		} else if (IsFreebsd ()) {
			rc = sys_umtx_timedwait_uint (w, expect, pshare, clock, timeout);
		} else {
			if (IsOpenbsd()) {
				// OpenBSD 6.8 futex() returns errors as
				// positive numbers, without setting CF.
				// This irregularity is fixed in 7.2 but
				// unfortunately OpenBSD futex() defines
				// its own ECANCELED condition, and that
				// overlaps with our system call wrapper
				if (pt) pt->pt_flags &= ~PT_OPENBSD_KLUDGE;
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
				    pt && (~pt->pt_flags & PT_OPENBSD_KLUDGE)) {
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
		rc = nsync_futex_polyfill_ (w, expect, clock, timeout);
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
	int rc, op, fop;

	ASSERT (count == 1 || count == INT_MAX);

	cosmo_once (&nsync_futex_.once, nsync_futex_init_);

	op = FUTEX_WAKE;
	if (pshare == PTHREAD_PROCESS_PRIVATE)
		op |= nsync_futex_.FUTEX_PRIVATE_FLAG_;

	if (nsync_futex_.is_supported) {
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
		} else if (IsXnu ()) {
			uint32_t op;
			if (pshare) {
				op = UL_COMPARE_AND_WAIT_SHARED;
			} else {
				op = UL_COMPARE_AND_WAIT;
			}
			if (count > 1) {
				op |= ULF_WAKE_ALL;
			}
			rc = ulock_wake (op, w, 0);
			ASSERT (!rc || rc == -ENOENT);
			if (!rc) {
				rc = 1;
			} else if (rc == -ENOENT) {
				rc = 0;
			}
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
		pthread_yield_np ();
		rc = 0;
	}

	STRACE ("futex(%t [%d], %s, %d) → %d woken",
		w, atomic_load_explicit (w, memory_order_relaxed),
		DescribeFutexOp (op), count, rc);

	return rc;
}
