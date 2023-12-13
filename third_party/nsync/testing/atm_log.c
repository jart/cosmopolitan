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
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"

#ifndef NSYNC_ATM_LOG
#define NSYNC_ATM_LOG 0
#endif

struct atm_log {
	uintptr_t i;
	uintptr_t thd_id;
	uintptr_t c;
	void *p;
	uintptr_t o;
	uintptr_t n;
	const char *file;
	uintptr_t line;
};

#define LOG_N 14

static struct atm_log log_entries[1 << LOG_N];
static uint32_t log_i;

static pthread_mutex_t log_mu;

static pthread_key_t key;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static void do_once (void) {
	pthread_mutex_init (&log_mu, NULL);
	pthread_key_create (&key, NULL);
}
static int thread_id;

void nsync_atm_log_ (int c, void *p, uint32_t o, uint32_t n, const char *file, int line) {
	if (NSYNC_ATM_LOG) {
		struct atm_log *e;
		uint32_t i;
		int *pthd_id;
		int thd_id;

		pthread_once (&once, &do_once);
		pthd_id = (int *) pthread_getspecific (key);

		pthread_mutex_lock (&log_mu);
		i = log_i++;
		if (pthd_id == NULL) {
			thd_id = thread_id++;
			pthd_id = (int *) malloc (sizeof (*pthd_id));
			pthread_setspecific (key, pthd_id);
			*pthd_id = thd_id;
		} else {
			thd_id = *pthd_id;
		}
		pthread_mutex_unlock (&log_mu);

		e = &log_entries[i & ((1 << LOG_N) - 1)];
		e->i = i;
		e->thd_id = thd_id;
		e->c = c;
		e->p = p;
		e->o = o;
		e->n = n;
		e->file = file;
		e->line = line;
	}
}

void nsync_atm_log_print_ (void) {
	if (NSYNC_ATM_LOG) {
		uint32_t i;
		pthread_once (&once, &do_once);
		pthread_mutex_lock (&log_mu);
		for (i = 0; i != (1 << LOG_N); i++) {
			struct atm_log *e = &log_entries[i];
			if (e->file != 0) {
				fprintf (stderr, "%6lx %3d %c  p %16p  o %8x  n %8x  %10s:%d\n",
					(unsigned long) e->i,
					(int) e->thd_id,
					e->c <= ' '? '?' : (char)e->c,
					e->p,
					(uint32_t) e->o,
					(uint32_t) e->n,
					e->file,
					(int) e->line);
			}
		}
		pthread_mutex_unlock (&log_mu);
	}
}
