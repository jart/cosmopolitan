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
#include "third_party/nsync/atomic.h"
#include "third_party/nsync/atomic.internal.h"
#include "third_party/nsync/common.internal.h"
#include "libc/thread/thread.h"

asm(".ident\t\"\\n\\n\
*NSYNC (Apache 2.0)\\n\
Copyright 2016 Google, Inc.\\n\
https://github.com/google/nsync\"");
// clang-format off

static pthread_key_t waiter_key;
static nsync_atomic_uint32_ pt_once;

static void do_once (nsync_atomic_uint32_ *ponce, void (*dest) (void *)) {
	uint32_t o = ATM_LOAD_ACQ (ponce);
	if (o != 2) {
		while (o == 0 && !ATM_CAS_ACQ (ponce, 0, 1)) {
			o = ATM_LOAD (ponce);
		}
		if (o == 0) {
			pthread_key_create (&waiter_key, dest);
			ATM_STORE_REL (ponce, 2);
		}
		while (ATM_LOAD_ACQ (ponce) != 2) {
			nsync_yield_ ();
		}
	}
}

void *nsync_per_thread_waiter_ (void (*dest) (void *)) {
	do_once (&pt_once, dest);
	return (pthread_getspecific (waiter_key));
}

void nsync_set_per_thread_waiter_ (void *v, void (*dest) (void *)) {
	do_once (&pt_once, dest);
	pthread_setspecific (waiter_key, v);
}
