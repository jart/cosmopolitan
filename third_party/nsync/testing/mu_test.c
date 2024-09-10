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
#include "third_party/nsync/testing/mu_test.inc"

int main (int argc, char *argv[]) {
	testing_base tb = testing_new (argc, argv, 0);

	TEST_RUN (tb, test_rlock);
	TEST_RUN (tb, test_mu_nthread);

	BENCHMARK_RUN (tb, benchmark_mu_contended);
	BENCHMARK_RUN (tb, benchmark_mutex_contended);
	BENCHMARK_RUN (tb, benchmark_xmutex_contended);
	BENCHMARK_RUN (tb, benchmark_wmutex_contended);

	BENCHMARK_RUN (tb, benchmark_mu_uncontended);
	BENCHMARK_RUN (tb, benchmark_rmu_uncontended);
	BENCHMARK_RUN (tb, benchmark_mutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_xmutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_wmutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_rmutex_uncontended);
	BENCHMARK_RUN (tb, benchmark_mu_uncontended_waiter);
	BENCHMARK_RUN (tb, benchmark_mu_uncontended_no_wakeup);
	BENCHMARK_RUN (tb, benchmark_rmu_uncontended_waiter);

	return (testing_base_exit (tb));
}
