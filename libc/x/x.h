/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#ifndef COSMOPOLITAN_LIBC_X_H_
#define COSMOPOLITAN_LIBC_X_H_
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/fmt/pflink.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis                                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  Standard Library veneers for folks not building embedded RTOS */

#define _XPNN   paramsnonnull()
#define _XRET   nothrow nocallback nodiscard returnsnonnull
#define _XMAL   returnspointerwithnoaliases _XRET
#define _XMALPG returnsaligned((PAGESIZE)) _XMAL

struct FILE;
struct sigaction;
struct timeval;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » system calls                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int xsigaction(int, void *, uint64_t, uint64_t, struct sigaction *);
int xwrite(int, const void *, uint64_t);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » memory                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *xdtoa(double) _XMAL;
char *xasprintf(const char *, ...) printfesque(1) paramsnonnull((1)) _XMAL;
char *xvasprintf(const char *, va_list) _XPNN _XMAL;
char *xgetline(struct FILE *) _XPNN mallocesque;
void *xmalloc(size_t) attributeallocsize((1)) _XMAL;
void *xrealloc(void *, size_t) attributeallocsize((2)) _XRET;
void *xcalloc(size_t, size_t) attributeallocsize((1, 2)) _XMAL;
void *xvalloc(size_t) attributeallocsize((1)) _XMALPG;
void *xmemalign(size_t, size_t) attributeallocalign((1))
    attributeallocsize((2)) _XMAL;
char *xstrdup(const char *) _XPNN _XMAL;
char *xstrndup(const char *, size_t) _XPNN _XMAL;
char *xstrcat(const char *, ...) paramsnonnull((1)) nullterminated() _XMAL;
char *xstrmul(const char *, size_t) paramsnonnull((1)) _XMAL;
char *xinet_ntop(int, const void *) _XPNN _XMAL;
char *xaescapec(const char *) _XPNN _XMAL;
char *xaescapesh(const char *) _XPNN _XMAL;
char *xaescapeshq(const char *) _XPNN _XMAL;
char *xaescape(const char *, int (*)(char *, unsigned, const char *,
                                     unsigned)) _XPNN hidden _XMAL;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » time                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *xiso8601i(int) mallocesque;
char *xiso8601tv(struct timeval *) mallocesque;
char *xiso8601ts(struct timespec *) mallocesque;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » input / output                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *xslurp(const char *) _XPNN _XMALPG nodiscard;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » safety                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define xstrcat(...) (xstrcat)(__VA_ARGS__, NULL)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » generic typing                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if __STDC_VERSION__ + 0 >= 201112

#define xiso8601(TS) \
  _Generic(*(TS), struct timeval : xiso8601tv, default : xiso8601ts)(TS)

#endif /* C11 */

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § eXtended apis » link-time optimizations                   ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define xasprintf(FMT, ...) (xasprintf)(PFLINK(FMT), ##__VA_ARGS__)
#define xvasprintf(FMT, VA) (xvasprintf)(PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_H_ */
