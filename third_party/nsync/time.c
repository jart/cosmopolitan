/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2016 Google Inc.                                                   │
│                                                                              │
│ Licensed under the Apache License, Version 2.0 (the "License");              │
│ you may not use this file except in compliance with the License.             │
│ You may obtain a copy of the License at                                      │
│                                                                              │
│     http://www.apache.org/licenses/LICENSE-2.0                               │
│                                                                              │
│ Unless required by applicable law or agreed to in writing, software          │
│ distributed under the License is distributed on an "AS IS" BASIS,            │
│ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     │
│ See the License for the specific language governing permissions and          │
│ limitations under the License.                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/struct/timespec.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "third_party/nsync/time.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");
// clang-format off

#define NSYNC_NS_IN_S_ (1000 * 1000 * 1000)

/* Return the maximum t, assuming it's an integral
   type, and the representation is not too strange.  */
#define MAX_INT_TYPE(t) (((t)~(t)0) > 1?   /*is t unsigned?*/ \
                (t)~(t)0 :  /*unsigned*/ \
                (t) ((((uintmax_t)1) << (sizeof (t) * CHAR_BIT - 1)) - 1)) /*signed*/

const nsync_time nsync_time_no_deadline =
        NSYNC_TIME_STATIC_INIT (MAX_INT_TYPE (int64_t), NSYNC_NS_IN_S_ - 1);

const nsync_time nsync_time_zero = NSYNC_TIME_STATIC_INIT (0, 0);

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
