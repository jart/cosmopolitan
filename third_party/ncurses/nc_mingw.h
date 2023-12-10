/****************************************************************************
 * Copyright 2018-2019,2020 Thomas E. Dickey                                *
 * Copyright 2008-2010,2017 Free Software Foundation, Inc.                  *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/

/****************************************************************************
 * Author: Thomas Dickey, 2008-on                                           *
 ****************************************************************************/

/* $Id: nc_mingw.h,v 1.9 2020/07/11 22:13:19 tom Exp $ */

#ifndef NC_MINGW_H
#define NC_MINGW_H 1

#ifdef _WIN32

#ifdef WINVER
#  if WINVER < 0x0501
#    error WINVER must at least be 0x0501
#  endif
#else
#  define WINVER 0x0501
#endif
#include <windows.h>

#undef sleep
#define sleep(n) Sleep((n) * 1000)

#undef gettimeofday
#define gettimeofday(tv,tz) _nc_gettimeofday(tv,tz)

#if HAVE_SYS_TIME_H
#include <sys/time.h>		/* for struct timeval */
#endif

#ifdef _MSC_VER
#include <winsock2.h>		/* for struct timeval */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ncurses_dll.h"

NCURSES_EXPORT(int) _nc_gettimeofday(struct timeval *, void *);

#undef HAVE_GETTIMEOFDAY
#define HAVE_GETTIMEOFDAY 1

#define SIGHUP  1
#define SIGKILL 9
#define getlogin() "username"

#undef wcwidth
#define wcwidth(ucs) _nc_wcwidth((wchar_t)(ucs))
NCURSES_EXPORT(int) _nc_wcwidth(wchar_t);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#endif /* NC_MINGW_H */
