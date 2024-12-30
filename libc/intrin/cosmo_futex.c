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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/ulock.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/futex.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
// clang-format off

#define FUTEX_WAIT_BITS_ FUTEX_BITSET_MATCH_ANY

errno_t cosmo_futex_thunk (atomic_int *, int, int, const struct timespec *, int *, int);
errno_t _futex_wake (atomic_int *, int, int) asm ("cosmo_futex_thunk");
int sys_futex_cp (atomic_int *, int, int, const struct timespec *, int *, int);

static struct CosmoFutex {
	atomic_uint once;
	int FUTEX_WAIT_;
	int FUTEX_PRIVATE_FLAG_;
	int FUTEX_CLOCK_REALTIME_;
	bool is_supported;
	bool timeout_is_relative;
} g_cosmo_futex;

static void cosmo_futex_init (void) {
	int e;
	atomic_int x;

	g_cosmo_futex.FUTEX_WAIT_ = FUTEX_WAIT;

	if (IsWindows ()) {
		g_cosmo_futex.is_supported = true;
		return;
	}

	if (IsXnu ()) {
		g_cosmo_futex.is_supported = true;
		g_cosmo_futex.timeout_is_relative = true;
		return;
	}

	if (IsFreebsd ()) {
		g_cosmo_futex.is_supported = true;
		g_cosmo_futex.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		return;
	}

	if (!(g_cosmo_futex.is_supported = IsLinux () || IsOpenbsd ()))
		return;

	// In our testing, we found that the monotonic clock on various
	// popular systems (such as Linux, and some BSD variants) was no
	// better behaved than the realtime clock, and routinely took
	// large steps backwards, especially on multiprocessors. Given
	// that "monotonic" doesn't seem to mean what it says,
	// implementers of cosmo_time might consider retaining the
	// simplicity of a single epoch within an address space, by
	// configuring any time synchronization mechanism (like ntp) to
	// adjust for leap seconds by adjusting the rate, rather than
	// with a backwards step.
	e = errno;
	atomic_store_explicit (&x, 0, memory_order_relaxed);
	if (IsLinux () &&
	    cosmo_futex_thunk (&x, FUTEX_WAIT_BITSET | FUTEX_CLOCK_REALTIME,
			       1, 0, 0, FUTEX_BITSET_MATCH_ANY) == -EAGAIN) {
		g_cosmo_futex.FUTEX_WAIT_ = FUTEX_WAIT_BITSET;
		g_cosmo_futex.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		g_cosmo_futex.FUTEX_CLOCK_REALTIME_ = FUTEX_CLOCK_REALTIME;
	} else if (IsOpenbsd () ||
		   (IsLinux () &&
		    !_futex_wake (&x, FUTEX_WAKE_PRIVATE, 1))) {
		g_cosmo_futex.FUTEX_WAIT_ = FUTEX_WAIT;
		g_cosmo_futex.FUTEX_PRIVATE_FLAG_ = FUTEX_PRIVATE_FLAG;
		g_cosmo_futex.timeout_is_relative = true;
	} else {
		g_cosmo_futex.FUTEX_WAIT_ = FUTEX_WAIT;
		g_cosmo_futex.timeout_is_relative = true;
	}
	errno = e;
}

static uint32_t cosmo_time_64to32u (uint64_t duration) {
	if (duration <= -1u)
		return duration;
	return -1u;
}

static int cosmo_futex_polyfill (atomic_int *w, int expect, int clock,
				 struct timespec *abstime) {
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

static int cosmo_futex_wait_win32 (atomic_int *w, int expect, char pshare,
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
		ok = WaitOnAddress (w, &expect, sizeof(int), cosmo_time_64to32u (timespec_tomillis (wait)));
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
			unassert (GetLastError () == ETIMEDOUT);
		}
	}
#else
	return 0;
#endif /* __x86_64__ */
}

static int cosmo_futex_fix_timeout (struct timespec *memory, int clock,
				    const struct timespec *abstime,
				    struct timespec **result) {
	struct timespec now;
	if (!abstime) {
		*result = 0;
		return 0;
	} else if (!g_cosmo_futex.timeout_is_relative) {
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

/**
 * Waits on futex.
 *
 * This function may be used to ask the OS to park the calling thread
 * until cosmo_futex_wake() is called on the memory address `w`.
 *
 * @param w is your futex
 * @param expect is the value `*w` is expected to have on entry
 * @param pshare is `PTHREAD_PROCESS_PRIVATE` / `PTHREAD_PROCESS_SHARED`
 * @param clock is `CLOCK_MONOTONIC`, `CLOCK_REALTIME`, etc.
 * @param abstime is null to wait forever or absolute timestamp to stop
 * @return 0 on success, or -errno on error
 * @raise EINVAL on bad parameter
 * @raise EAGAIN if `*w` wasn't `expect`
 * @raise EINTR if a signal handler was called while waiting
 * @raise ECANCELED if calling thread was canceled while waiting
 * @cancelationpoint
 */
int cosmo_futex_wait (atomic_int *w, int expect, char pshare,
		      int clock, const struct timespec *abstime) {
	int e, rc, op;
	struct CosmoTib *tib;
	struct PosixThread *pt;
	struct timespec tsmem;
	struct timespec *timeout = 0;
	BEGIN_CANCELATION_POINT;

	cosmo_once (&g_cosmo_futex.once, cosmo_futex_init);

	op = g_cosmo_futex.FUTEX_WAIT_;
	if (pshare == PTHREAD_PROCESS_PRIVATE)
		op |= g_cosmo_futex.FUTEX_PRIVATE_FLAG_;
	if (clock == CLOCK_REALTIME ||
	    clock == CLOCK_REALTIME_COARSE)
		op |= g_cosmo_futex.FUTEX_CLOCK_REALTIME_;

	if (abstime && timespec_cmp (*abstime, timespec_zero) <= 0) {
		rc = -ETIMEDOUT;
		goto Finished;
	}

	if (atomic_load_explicit (w, memory_order_acquire) != expect) {
		rc = -EAGAIN;
		goto Finished;
	}

	if ((rc = cosmo_futex_fix_timeout (&tsmem, clock, abstime, &timeout)))
		goto Finished;

	LOCKTRACE ("futex(%t [%d], %s, %#x, %s) → ...",
		   w, atomic_load_explicit (w, memory_order_relaxed),
		   DescribeFutexOp (op), expect,
		   DescribeTimespec (0, timeout));

	tib = __get_tls();
	pt = (struct PosixThread *)tib->tib_pthread;

	if (g_cosmo_futex.is_supported) {
		e = errno;
		if (IsWindows ()) {
			// Windows 8 futexes don't support multiple processes :(
			if (pshare) goto Polyfill;
			sigset_t m = __sig_block ();
			rc = cosmo_futex_wait_win32 (w, expect, pshare, clock, timeout, pt, m);
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
				us = cosmo_time_64to32u (timespec_tomicros (*timeout));
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
		rc = cosmo_futex_polyfill (w, expect, clock, timeout);
	}

Finished:
	STRACE ("futex(%t [%d], %s, %#x, %s) → %s",
		w, atomic_load_explicit (w, memory_order_relaxed),
		DescribeFutexOp (op), expect,
		DescribeTimespec (0, abstime),
		DescribeErrno (rc));

	END_CANCELATION_POINT;
	return rc;
}

/**
 * Wakes futex.
 *
 * @param w is your futex
 * @param count is number of threads to wake (usually 1 or `INT_MAX`)
 * @param pshare is `PTHREAD_PROCESS_PRIVATE` / `PTHREAD_PROCESS_SHARED`
 * @return number of threads woken on success, or -errno on error
 */
int cosmo_futex_wake (atomic_int *w, int count, char pshare) {
	int rc, op, fop;

	cosmo_once (&g_cosmo_futex.once, cosmo_futex_init);

	op = FUTEX_WAKE;
	if (pshare == PTHREAD_PROCESS_PRIVATE)
		op |= g_cosmo_futex.FUTEX_PRIVATE_FLAG_;

	if (g_cosmo_futex.is_supported) {
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
			unassert (!rc || rc == -ENOENT);
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
