/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/weirdtypes.h"
#include "libc/time.h"

char *
ctime_r(const time_t *timep, char *buf)
{
	struct tm mytm;
	struct tm *tmp = localtime_r(timep, &mytm);
	return tmp ? asctime_r(tmp, buf) : NULL;
}
