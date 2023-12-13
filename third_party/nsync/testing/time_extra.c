/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
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
#include "libc/calls/weirdtypes.h"
#include "libc/errno.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/time_extra.h"
#include "third_party/nsync/time.h"

char *nsync_time_str (nsync_time t, int decimals) {
	static const struct {
		const char *suffix;
		double multiplier;
	} scale[] = {
		{ "ns", 1.0e-9, },
		{ "us", 1e-6, },
		{ "ms", 1e-3, },
		{ "s", 1.0, },
		{ "hr", 3600.0, },
	};
        double s = nsync_time_to_dbl (t);
	int i = 0;
	while (i + 1 != sizeof (scale) / sizeof (scale[0]) && scale[i + 1].multiplier <= s) {
		i++;
	}
	return (smprintf ("%.*f%s", decimals, s/scale[i].multiplier, scale[i].suffix));
}

double nsync_time_to_dbl (nsync_time t) {
	return (((double) NSYNC_TIME_SEC (t)) + ((double) NSYNC_TIME_NSEC (t) * 1e-9));
}

nsync_time nsync_time_from_dbl (double d) {
	time_t s = (time_t) d;
	if (d < s) {
		s--;
	}
	return (nsync_time_s_ns (s, (unsigned) ((d - (double) s) * 1e9)));
}
