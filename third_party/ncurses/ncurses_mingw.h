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

/* $Id: ncurses_mingw.h,v 1.7 2021/06/17 21:26:02 tom Exp $ */

/*
 * This is a placeholder up to now and describes what needs to be implemented
 * to support I/O to external terminals with ncurses on the Windows OS.
 */

#ifdef _WIN32
#ifndef _NC_MINGWH
#define _NC_MINGWH

#define USE_CONSOLE_DRIVER 1

#undef  TERMIOS
#define TERMIOS 1

typedef unsigned char cc_t;
typedef unsigned int  tcflag_t;
typedef unsigned int  speed_t;
typedef unsigned short otcflag_t;
typedef unsigned char ospeed_t;

#define NCCS 18
struct termios
{
  tcflag_t	c_iflag;
  tcflag_t	c_oflag;
  tcflag_t	c_cflag;
  tcflag_t	c_lflag;
  char		c_line;
  cc_t		c_cc[NCCS];
  speed_t	c_ispeed;
  speed_t	c_ospeed;
};

extern NCURSES_EXPORT(int)  _nc_mingw_tcsetattr(
    int fd,
    int optional_actions,
    const struct termios* arg);
extern NCURSES_EXPORT(int)  _nc_mingw_tcgetattr(
    int fd,
    struct termios* arg);
extern NCURSES_EXPORT(int)  _nc_mingw_tcflush(
    int fd,
    int queue);
extern NCURSES_EXPORT(void) _nc_set_term_driver(void* term);

#endif /* _NC_MINGWH */
#endif /* _WIN32 */
