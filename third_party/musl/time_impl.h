#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_TIME_IMPL_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_TIME_IMPL_H_
#include "libc/time.h"
#include "libc/str/locale.h"
#include "libc/calls/weirdtypes.h"
COSMOPOLITAN_C_START_

int __days_in_month(int, int);
int __month_to_secs(int, int);
long long __year_to_secs(long long, int *);
long long __tm_to_secs(const struct tm *);
const char *__tm_to_tzname(const struct tm *);
int __secs_to_tm(long long, struct tm *);
void __secs_to_zone(long long, int, int *, long *, long *, const char **);
const char *__strftime_fmt_1(char (*)[100], size_t *, int, const struct tm *, locale_t, int);
extern const char __utc[];

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_TIME_IMPL_H_ */
