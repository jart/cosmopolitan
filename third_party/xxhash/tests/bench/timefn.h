/*
 * Copyright (c) 2016-2021 Yann Collet, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

#ifndef TIME_FN_H_MODULE_287987
#define TIME_FN_H_MODULE_287987

#if defined (__cplusplus)
extern "C" {
#endif


/*-****************************************
*  Dependencies
******************************************/
#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"
#include "libc/thread/thread.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/weirdtypes.h"
#include "libc/intrin/newbie.h"
#include "libc/sock/select.h"
#include "libc/sysv/consts/endian.h"    /* utime */
#if defined(_MSC_VER)
// MISSING #include <sys/utime.h>  /* utime */
#else
#include "libc/time/struct/utimbuf.h"
#include "libc/time/time.h"      /* utime */
#endif
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/sched.h"
#include "libc/sysv/consts/timer.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"         /* clock_t, clock, CLOCKS_PER_SEC */



/*-****************************************
*  Local Types
******************************************/

#if !defined (__VMS) && (defined (__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */) )
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/literal.h"
  typedef uint64_t           PTime;  /* Precise Time */
#else
  typedef unsigned long long PTime;  /* does not support compilers without long long support */
#endif



/*-****************************************
*  Time functions
******************************************/
#if defined(_WIN32)   /* Windows */

    // MISSING #include <Windows.h>   /* LARGE_INTEGER */
    typedef LARGE_INTEGER UTIL_time_t;
    #define UTIL_TIME_INITIALIZER { { 0, 0 } }

#elif defined(__APPLE__) && defined(__MACH__)

    // MISSING #include <mach/mach_time.h>
    typedef PTime UTIL_time_t;
    #define UTIL_TIME_INITIALIZER 0

#elif (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) /* C11 */) \
    && defined(TIME_UTC) /* C11 requires timespec_get, but FreeBSD 11 lacks it, while still claiming C11 compliance */

    typedef struct timespec UTIL_time_t;
    #define UTIL_TIME_INITIALIZER { 0, 0 }

#else   /* relies on standard C90 (note : clock_t measurements can be wrong when using multi-threading) */

    typedef clock_t UTIL_time_t;
    #define UTIL_TIME_INITIALIZER 0

#endif


UTIL_time_t UTIL_getTime(void);
PTime UTIL_getSpanTimeMicro(UTIL_time_t clockStart, UTIL_time_t clockEnd);
PTime UTIL_getSpanTimeNano(UTIL_time_t clockStart, UTIL_time_t clockEnd);

#define SEC_TO_MICRO ((PTime)1000000)
PTime UTIL_clockSpanMicro(UTIL_time_t clockStart);
PTime UTIL_clockSpanNano(UTIL_time_t clockStart);

void UTIL_waitForNextTick(void);


#if defined (__cplusplus)
}
#endif

#endif /* TIME_FN_H_MODULE_287987 */
