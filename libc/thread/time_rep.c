// clang-format off
/* Copyright 2016 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License. */

#include "libc/thread/nsync_cpp.h"
#include "libc/thread/platform.h"
#include "libc/thread/compiler.h"
#include "libc/thread/cputype.h"
#include "libc/thread/nsync_time_init.h"
#include "libc/thread/nsync_time.h"

NSYNC_CPP_START_

#define NSYNC_NS_IN_S_ (1000 * 1000 * 1000)

/* Return the maximum t, assuming it's an integral
   type, and the representation is not too strange.  */
#define MAX_INT_TYPE(t) (((t)~(t)0) > 1?   /*is t unsigned?*/ \
                (t)~(t)0 :  /*unsigned*/ \
                (t) ((((uintmax_t)1) << (sizeof (t) * CHAR_BIT - 1)) - 1)) /*signed*/

const nsync_time nsync_time_no_deadline =
        NSYNC_TIME_STATIC_INIT (MAX_INT_TYPE (time_t), NSYNC_NS_IN_S_ - 1);

const nsync_time nsync_time_zero = NSYNC_TIME_STATIC_INIT (0, 0);

nsync_time nsync_time_s_ns (time_t s, unsigned ns) {
	nsync_time t;
	memset (&t, 0, sizeof (t));
	t.tv_sec = s;
	t.tv_nsec = ns;
	return (t);
}

nsync_time nsync_time_now (void) {
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
	return (ts);
}

nsync_time nsync_time_sleep (nsync_time delay) {
	struct timespec ts;
	struct timespec remain;
	memset (&ts, 0, sizeof (ts));
	ts.tv_sec = NSYNC_TIME_SEC (delay);
	ts.tv_nsec = NSYNC_TIME_NSEC (delay);
	if (nanosleep (&ts, &remain) == 0) {
		/* nanosleep() is not required to fill in "remain"
		   if it returns 0. */
		memset (&remain, 0, sizeof (remain));
	}
	return (remain);
}

nsync_time nsync_time_add (nsync_time a, nsync_time b) {
	a.tv_sec += b.tv_sec;
	a.tv_nsec += b.tv_nsec;
	if (a.tv_nsec >= NSYNC_NS_IN_S_) {
		a.tv_nsec -= NSYNC_NS_IN_S_;
		a.tv_sec++;
	}
	return (a);
}

nsync_time nsync_time_sub (nsync_time a, nsync_time b) {
	a.tv_sec -= b.tv_sec;
	if (a.tv_nsec < b.tv_nsec) {
		a.tv_nsec += NSYNC_NS_IN_S_;
		a.tv_sec--;
	}
	a.tv_nsec -= b.tv_nsec;
	return (a);
}

int nsync_time_cmp (nsync_time a, nsync_time b) {
	int cmp = (NSYNC_TIME_SEC (a) > NSYNC_TIME_SEC (b)) -
		  (NSYNC_TIME_SEC (a) < NSYNC_TIME_SEC (b));
	if (cmp == 0) {
		cmp = (NSYNC_TIME_NSEC (a) > NSYNC_TIME_NSEC (b)) -
		      (NSYNC_TIME_NSEC (a) < NSYNC_TIME_NSEC (b));
	}
	return (cmp);
}

NSYNC_CPP_END_
