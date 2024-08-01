/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/time.h"

char *asctime(const struct tm *tm)
{
	static char buf[26];
	return asctime_r(tm, buf);
}
