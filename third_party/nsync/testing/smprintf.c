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
#include "third_party/nsync/testing/smprintf.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

char *smprintf (const char *fmt, ...) {
	int m = strlen (fmt) * 2 + 1;
	char *buf = (char *) malloc (m);
	int didnt_fit;
	do {
		va_list ap;
		int x;
		va_start (ap, fmt);
		x = vsnprintf (buf, m, fmt, ap);
		va_end (ap);
		if (x >= m) {
			buf = (char *) realloc (buf, m = x+1);
			didnt_fit = 1;
		} else if (x < 0 || x == m-1) {
			buf = (char *) realloc (buf, m *= 2);
			didnt_fit = 1;
		} else {
			didnt_fit = 0;
		}
	} while (didnt_fit);
	return (buf);
}
