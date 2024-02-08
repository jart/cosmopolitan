/****************************************************************************
 * Copyright 2018-2020,2021 Thomas E. Dickey                                *
 * Copyright 2008-2014,2017 Free Software Foundation, Inc.                  *
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
 * Author: Juergen Pfeifer, 2008-on                                         *
 ****************************************************************************/

/* $Id: win32_curses.h,v 1.3 2021/06/17 21:26:02 tom Exp $ */

/*
 * This is the interface we use on Windows to mimic the control of the settings
 * of what acts like the classic TTY - the Windows Console.
 */

#if (defined(_WIN32) || defined(_WIN64))
#ifndef _NC_WIN32_CURSES_H
#define _NC_WIN32_CURSES_H 1

struct winconmode
{
  unsigned long dwFlagIn;
  unsigned long dwFlagOut;
};

extern NCURSES_EXPORT(void*) _nc_console_fd2handle(int fd);
extern NCURSES_EXPORT(int)   _nc_console_setmode(void* handle, const struct winconmode* arg);
extern NCURSES_EXPORT(int)   _nc_console_getmode(void* handle, struct winconmode* arg);
extern NCURSES_EXPORT(int)   _nc_console_flush(void* handle);

/*
  A few definitions of Unix functions we need to emulate
*/
#define SIGHUP  1
#define SIGKILL 9

#undef  getlogin
#define getlogin() getenv("USERNAME")

#undef  ttyname
#define ttyname(fd) NULL

#undef sleep
#define sleep(n) Sleep((n) * 1000)

#undef gettimeofday
#define gettimeofday(tv,tz) _nc_gettimeofday(tv,tz)

#endif /* _NC_WIN32_CURSES_H */
#endif /* _WIN32||_WIN64 */
