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
#include "third_party/nsync/counter.h"
#include "third_party/nsync/testing/closure.h"
#include "third_party/nsync/testing/smprintf.h"
#include "third_party/nsync/testing/testing.h"
#include "third_party/nsync/testing/time_extra.h"

/* Verify the properties of a zero counter. */
static void test_counter_zero (testing t) {
	int i;
	nsync_counter c = nsync_counter_new (0);
	for (i = 0; i != 2; i++) {
		if (nsync_counter_value (c) != 0) {
			TEST_ERROR (t, ("zero counter is not zero (test, %d)", i));
		}
		if (nsync_counter_wait (c, nsync_time_zero) != 0) {
			TEST_ERROR (t, ("zero counter is not zero (poll, %d)", i));
		}
		if (nsync_counter_wait (c, nsync_time_no_deadline) != 0) {
			TEST_ERROR (t, ("zero counter is not zero (infinite wait, %d)", i));
		}
		nsync_counter_add (c, 0);
	}
	nsync_counter_free (c);
}


/* Verify the properties of a non-zero counter. */
static void test_counter_non_zero (testing t) {
	nsync_time start;
	nsync_time waited;
	nsync_time abs_deadline;
	nsync_counter c = nsync_counter_new (1);
	if (nsync_counter_value (c) != 1) {
		TEST_ERROR (t, ("counter is not 1 (test)"));
	}
	if (nsync_counter_wait (c, nsync_time_zero) != 1) {
		TEST_ERROR (t, ("counter is not 1 (poll)"));
	}
	start = nsync_time_now ();
	abs_deadline = nsync_time_add (nsync_time_now (), nsync_time_ms (1000));
	if (nsync_counter_wait (c, abs_deadline) != 1) {
		TEST_ERROR (t, ("counter is not 1 (1s wait)"));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("timed wait on non-zero counter returned too quickly (1s wait took %s)",
			   nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("timed wait on non-zero counter returned too slowly (1s wait took %s)",
			   nsync_time_str (waited, 2)));
	}

	if (nsync_counter_add (c, -1) != 0) {
		TEST_ERROR (t, ("zero counter note is not 0 (add)"));
	}

	if (nsync_counter_value (c) != 0) {
		TEST_ERROR (t, ("zero counter note is not 0 (test)"));
	}
	if (nsync_counter_wait (c, nsync_time_zero) != 0) {
		TEST_ERROR (t, ("zero counter note is not 0 (poll)"));
	}
	if (nsync_counter_wait (c, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("zero counter note is not 0 (infinite wait)"));
	}
	nsync_counter_free (c);
}

static void decrement_at (nsync_counter c, nsync_time abs_deadline) {
	nsync_time_sleep_until (abs_deadline);
	nsync_counter_add (c, -1);
}

CLOSURE_DECL_BODY2 (decrement, nsync_counter, nsync_time)

/* Test decrement of a counter. */
static void test_counter_decrement (testing t) {
	nsync_time start;
	nsync_time waited;
	nsync_counter c = nsync_counter_new (1);
	closure_fork (closure_decrement (&decrement_at, c,
		nsync_time_add (nsync_time_now (), nsync_time_ms (1000))));
	start = nsync_time_now ();
	if (nsync_counter_wait (c, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("counter is not 0"));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("counter wait too fast (1s delay took %s)", nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("counter wait too slow (1s delay took %s)", nsync_time_str (waited, 2)));
	}
	if (nsync_counter_value (c) != 0) {
		TEST_ERROR (t, ("counter is not 0 (test)"));
	}
	if (nsync_counter_wait (c, nsync_time_zero) != 0) {
		TEST_ERROR (t, ("counter is not 0 (poll)"));
	}
	if (nsync_counter_wait (c, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("counter is not 0 (infinite wait)"));
	}
	nsync_counter_free (c);

	c = nsync_counter_new (1);
	closure_fork (closure_decrement (&decrement_at, c,
		nsync_time_add (nsync_time_now (), nsync_time_ms (1000))));
	start = nsync_time_now ();
	while (nsync_counter_value (c) != 0) {
		nsync_time_sleep (nsync_time_ms (10));
	}
	waited = nsync_time_sub (nsync_time_now (), start);
	if (nsync_time_cmp (waited, nsync_time_ms (900)) < 0) {
		TEST_ERROR (t, ("counter wait too fast (1s delay took %s)", nsync_time_str (waited, 2)));
	}
	if (nsync_time_cmp (waited, nsync_time_ms (2000)) > 0) {
		TEST_ERROR (t, ("counter wait too slow (1s delay took %s)", nsync_time_str (waited, 2)));
	}
	if (nsync_counter_value (c) != 0) {
		TEST_ERROR (t, ("counter is not 0 (test)"));
	}
	if (nsync_counter_wait (c, nsync_time_zero) != 0) {
		TEST_ERROR (t, ("counter is not 0 (poll)"));
	}
	if (nsync_counter_wait (c, nsync_time_no_deadline) != 0) {
		TEST_ERROR (t, ("counter is not 0 (infinite wait)"));
	}
	nsync_counter_free (c);
}

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);
	TEST_RUN (tb, test_counter_zero);
	TEST_RUN (tb, test_counter_non_zero);
	TEST_RUN (tb, test_counter_decrement);
	return (testing_base_exit (tb));
}
