/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (c) 1996, David Mazieres <dm@uun.org>                              │
│ Copyright (c) 2008, Damien Miller <djm@openbsd.org>                          │
│ Copyright (c) 2013, Markus Friedl <markus@openbsd.org>                       │
│ Copyright (c) 2014, Theo de Raadt <deraadt@openbsd.org>                      │
│ Copyright (c) Robert Clausecker <fuz@FreeBSD.org>                            │
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
#include "libc/calls/state.internal.h"
#include "libc/calls/typedef/u.h"
#include "libc/cosmo.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
// clang-format off

#define FAIR_SHARE 1024
#define USE_CURRENT_CPU 1
#define USE_THREAD_COUNT 1

__notice(arc4random_notice, "arc4random (ISC)\n\
Copyright (c) 1996, David Mazieres <dm@uun.org>\n\
Copyright (c) 2008, Damien Miller <djm@openbsd.org>\n\
Copyright (c) 2013, Markus Friedl <markus@openbsd.org>\n\
Copyright (c) 2014, Theo de Raadt <deraadt@openbsd.org>\n\
Copyright (c) 2024, Robert Clausecker <fuz@FreeBSD.org>\n\
Copyright (c) 2025, Justine Tunney <jtunney@gmail.com>\n\
Based upon D. J. Bernstein's public domain ChaCha code\n\
Based upon Daniel Lemire's arc4random_uniform research");

#define KEYSTREAM_ONLY
#include "libc/str/chacha.inc"

#define minimum(a, b) ((a) < (b) ? (a) : (b))

#define KEYSZ	32
#define IVSZ	8
#define BLOCKSZ	64
#define RSBUFSZ	(16*BLOCKSZ)

#define REKEY_BASE	(1024*1024) /* NB. should be a power of 2 */

/* This is zero'd out in forked children. */
struct _rs {
	size_t		rs_have;	/* valid bytes at end of rs_buf */
	size_t		rs_count;	/* bytes till reseed */
};

/* Optional to preserve in forked children. */
struct _rsx {
	chacha_ctx	rs_chacha;	/* chacha context for random keystream */
	u_char		rs_buf[RSBUFSZ];	/* keystream blocks */
};

struct _rng {
	alignas(64) pthread_mutex_t mtx;
	bool once;
	struct _rs rs;
	struct _rsx rsx;
};

static struct _rng _rs_state[COSMO_SHARDS];

void
__arc4random_fork_prepare(void)
{
	if (__isthreaded < 2)
		return;
	for (int i = 0; i < COSMO_SHARDS; ++i)
		if (_rs_state[i].once)
			pthread_mutex_lock(&_rs_state[i].mtx);
}

void
__arc4random_fork_parent(void)
{
	if (__isthreaded < 2)
		return;
	for (int i = COSMO_SHARDS; i--;)
		if (_rs_state[i].once)
			pthread_mutex_unlock(&_rs_state[i].mtx);
}

void
__arc4random_fork_child(void)
{
	for (int i = 0; i < COSMO_SHARDS; ++i) {
		if (!_rs_state[i].once)
			continue;
		pthread_mutex_wipe_np(&_rs_state[i].mtx);
		memset(&_rs_state[i].rs, 0, sizeof(_rs_state[i].rs));
	}
}

static inline void
_rs_init(struct _rng *rng, u_char *buf, size_t n)
{
	if (n < KEYSZ + IVSZ)
		return;
	chacha_keysetup(&rng->rsx.rs_chacha, buf, KEYSZ * 8);
	chacha_ivsetup(&rng->rsx.rs_chacha, buf + KEYSZ);
	rng->once = true;
}

static inline void
_rs_rekey(struct _rng *rng, u_char *dat, size_t datlen)
{
#ifndef KEYSTREAM_ONLY
	memset(rng->rsx.rs_buf, 0, sizeof(rng->rsx.rs_buf));
#endif
	/* fill rs_buf with the keystream */
	chacha_encrypt_bytes(&rng->rsx.rs_chacha, rng->rsx.rs_buf,
	    rng->rsx.rs_buf, sizeof(rng->rsx.rs_buf));
	/* mix in optional user provided data */
	if (dat) {
		size_t i, m;

		m = minimum(datlen, KEYSZ + IVSZ);
		for (i = 0; i < m; i++)
			rng->rsx.rs_buf[i] ^= dat[i];
	}
	/* immediately reinit for backtracking resistance */
	_rs_init(rng, rng->rsx.rs_buf, KEYSZ + IVSZ);
	memset(rng->rsx.rs_buf, 0, KEYSZ + IVSZ);
	rng->rs.rs_have = sizeof(rng->rsx.rs_buf) - KEYSZ - IVSZ;
}

static void
_rs_stir(struct _rng *rng)
{
	u_char rnd[KEYSZ + IVSZ];
	uint32_t rekey_fuzz = 0;

	if (getentropy(rnd, sizeof rnd) == -1)
		notpossible;

	if (!rng->once) {
		_rs_init(rng, rnd, sizeof(rnd));
	} else {
		_rs_rekey(rng, rnd, sizeof(rnd));
	}
	explicit_bzero(rnd, sizeof(rnd));	/* discard source seed */

	/* invalidate rs_buf */
	rng->rs.rs_have = 0;
	memset(rng->rsx.rs_buf, 0, sizeof(rng->rsx.rs_buf));

	/* rekey interval should not be predictable */
	chacha_encrypt_bytes(&rng->rsx.rs_chacha, (uint8_t *)&rekey_fuzz,
	    (uint8_t *)&rekey_fuzz, sizeof(rekey_fuzz));
	rng->rs.rs_count = REKEY_BASE + (rekey_fuzz % REKEY_BASE);
}

static inline void
_rs_stir_if_needed(struct _rng *rng, size_t len)
{
	if (!rng->once || rng->rs.rs_count <= len)
		_rs_stir(rng);
	if (rng->rs.rs_count <= len)
		rng->rs.rs_count = 0;
	else
		rng->rs.rs_count -= len;
}

static inline void
_rs_random_buf(struct _rng *rng, void *_buf, size_t n)
{
	u_char *buf = (u_char *)_buf;
	u_char *keystream;
	size_t m;

	_rs_stir_if_needed(rng, n);
	while (n > 0) {
		if (rng->rs.rs_have > 0) {
			m = minimum(n, rng->rs.rs_have);
			keystream = rng->rsx.rs_buf + sizeof(rng->rsx.rs_buf)
			    - rng->rs.rs_have;
			memcpy(buf, keystream, m);
			memset(keystream, 0, m);
			buf += m;
			n -= m;
			rng->rs.rs_have -= m;
		}
		if (rng->rs.rs_have == 0)
			_rs_rekey(rng, NULL, 0);
	}
}

/**
 * Fills buffer with random data.
 *
 * This is the best random number generator provided by cosmo libc.
 *
 * This RNG is simple. Unlike getrandom() which can fail in numerous
 * ways, this API always succeeds and returns the full number of bytes.
 *
 * This RNG is cryptographic grade. It uses D.J. Bernstein's ChaCha
 * algorithm. The code for this implementation was originally copied
 * from the OpenBSD and FreeBSD codebases.
 *
 * This RNG is fast. arc4random runs in userspace and occasionally
 * reseeds itself using getentropy(). This gives you a 10x performance
 * boost by avoiding costly system calls.
 *
 * This RNG is scalable. The Cosmopolitan Libc authors have modified the
 * original OpenBSD source code so it doesn't use a GIL. We instead load
 * balance threads across a fixed number of arc4random states which are
 * indexed based on the current cpu if possible otherwise the thread id.
 * This way threads needn't wait in sequence and can work concurrently.
 * The only other fast cryptographic userspace PRNG we've seen that can
 * do this is Microsoft's ProcessPrng() in Windows 10+.
 *
 * This RNG makes the single-threaded case go fast. Similar to FreeBSD
 * our integration with the C library enables us to know when it's safe
 * to avoid mutexes entirely. If your app never creates a thread, then
 * arc4random() will never need to pay the cost of locks.
 *
 * This RNG is fork() safe. Naive random number generators oftentimes
 * have an issue where forked child processes are likely to generate the
 * same numbers as the parent. Our libc system call wrapper ensures the
 * RNG state is appropriately wiped. However if you talk directly to the
 * kernel, using functions like sys_fork(), you're on your own. vfork()
 * should be safe, since the child process is actually just a thread.
 */
void
arc4random_buf(void *buf, size_t n)
{
	size_t i, w;
	struct _rng *rng;

	FORBIDDEN_IN_POSIX_SPAWN;

	/* load balance threads across fixed number of shards */
	rng = &_rs_state[cosmo_shard()];

	/* avoid one thread hogging shard */
	for (i = 0; i < n; i += w) {
		w = n - i;
		if (w > FAIR_SHARE)
			w = FAIR_SHARE;
		if (__isthreaded >= 2)
			pthread_mutex_lock(&rng->mtx);
		_rs_random_buf(rng, (char *)buf + i, w);
		if (__isthreaded >= 2)
			pthread_mutex_unlock(&rng->mtx);
	}
}

/**
 * Returns single random 32-bit value.
 *
 * The arc4random() function returns pseudo-random numbers in the range
 * of 0 to (2**32)−1, and has twice the range of rand(3) and random(3).
 *
 * This function is a wrapper around arc4random_buf() which can be used
 * to obtain an arbitrary number of bytes.
 *
 * Be careful about using the euclidean remainder operator (`%`) on the
 * result of this function. It's only safe to do that if you keep the
 * result unsigned and the denominator is a two power. What you almost
 * certainly want instead is arc4random_uniform() to do safe modulus.
 */
uint32_t
arc4random(void)
{
	uint32_t val;
	arc4random_buf(&val, sizeof(val));
	return val;
}

/**
 * Returns random number less than `upper_bound`.
 *
 * This is like arc4random() except the arc4random_uniform() function
 * will calculate a uniformly distributed random number that's less than
 * `upper_bound` while avoiding "modulo bias".
 *
 * This implementation uses the optimized technique discovered by Daniel
 * Lemire in his paper "Fast Random Integer Generation in an Interval",
 * Association for Computing Machinery, ACM Trans. Model. Comput.
 * Simul., no. 1, vol. 29, pp. 1--12, New York, NY, USA, January 2019.
 * Available: https://r-libre.teluq.ca/1437/1/rangedrng.pdf
 */
uint32_t
arc4random_uniform(uint32_t upper_bound)
{
	uint64_t product;

	/*
	 * The paper uses these variable names:
	 *
	 * L -- log2(UINT32_MAX+1)
	 * s -- upper_bound
	 * x -- arc4random() return value
	 * m -- product
	 * l -- (uint32_t)product
	 * t -- threshold
	 */

	if (upper_bound <= 1)
		return 0;

	product = upper_bound * (uint64_t)arc4random();

	if ((uint32_t)product < upper_bound) {
		uint32_t threshold;

		/* threshold = (2**32 - upper_bound) % upper_bound */
		threshold = -upper_bound % upper_bound;
		while ((uint32_t)product < threshold)
			product = upper_bound * (uint64_t)arc4random();
	}

	return product >> 32;
}
