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

/* $Id: nc_win32.h,v 1.9 2020/09/13 00:17:30 tom Exp $ */

#ifndef NC_WIN32_H
#define NC_WIN32_H 1

#if defined(_WIN32) || defined(_WIN64)

#ifndef _NC_WINDOWS
#define _NC_WINDOWS
#endif

#ifdef TERMIOS
#error TERMIOS must not be defined on Windows
#endif

/* We no longer support WindowsXP.
   Minimum requirement is Windows Vista or Server2008,
   aka Windows NT 6.0
*/
#ifdef WINVER
#  if WINVER < 0x0600
#    error WINVER must at least be 0x0600
#  endif
#else
#  define WINVER 0x0600
#endif

#undef _NC_CHECK_MINTTY
#if WINVER >= 0x0600
#define _NC_CHECK_MINTTY
#endif

#include <windows.h>

#if HAVE_SYS_TIME_H
#include <sys/time.h>		/* for struct timeval */
#endif

#ifdef _NC_MSC
#include <winsock2.h>		/* for struct timeval */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "ncurses_dll.h"

#undef HAVE_GETTIMEOFDAY
#define HAVE_GETTIMEOFDAY 1
extern NCURSES_EXPORT(int) _nc_gettimeofday(struct timeval *, void *);

#undef wcwidth
#define wcwidth(ucs)  _nc_wcwidth((wchar_t)(ucs))
extern NCURSES_EXPORT(int)    _nc_wcwidth(wchar_t);

#ifdef EVENTLIST_2nd /* test.priv.h just needs the preceding */

extern NCURSES_EXPORT(void)   _nc_console_size(int* Lines, int* Cols);
extern NCURSES_EXPORT(HANDLE) _nc_console_handle(int fd);
extern NCURSES_EXPORT(int)    _nc_console_isatty(int fd);
extern NCURSES_EXPORT(int)    _nc_console_test(int fd);
extern NCURSES_EXPORT(int)    _nc_console_read(SCREEN *sp,HANDLE hdl,int *buf);
extern NCURSES_EXPORT(int)    _nc_console_twait(SCREEN *, HANDLE,int,int,int * EVENTLIST_2nd(_nc_eventlist * evl));
extern NCURSES_EXPORT(WORD)   _nc_console_MapColor(bool fore, int color);
extern NCURSES_EXPORT(void)   _nc_console_selectActiveHandle(void);
extern NCURSES_EXPORT(bool)   _nc_console_get_SBI(void);
extern NCURSES_EXPORT(void)   _nc_console_set_scrollback(bool normal, CONSOLE_SCREEN_BUFFER_INFO * info);
extern NCURSES_EXPORT(int)    _nc_console_testmouse(SCREEN *,HANDLE,int EVENTLIST_2nd(_nc_eventlist*));
extern NCURSES_EXPORT(int)    _nc_console_keyok(int keycode,int flag);
extern NCURSES_EXPORT(bool)   _nc_console_keyExist(int keycode);
extern NCURSES_EXPORT(bool)   _nc_console_checkinit(bool initFlag, bool assumeTermInfo);
extern NCURSES_EXPORT(int)    _nc_console_vt_supported(void);

#ifdef _NC_CHECK_MINTTY
extern NCURSES_EXPORT(int)    _nc_console_checkmintty(int fd, LPHANDLE pMinTTY);
#endif

#undef VALID_TERM_ENV
#define MS_TERMINAL "ms-terminal"
#define VALID_TERM_ENV(term_env, no_terminal) \
	(term_env = (NonEmpty(term_env) \
		      ? term_env \
		      : (_nc_console_vt_supported() \
		         ? MS_TERMINAL \
		         : no_terminal)), \
	 NonEmpty(term_env))

  /*
   * Various Console mode definitions
   */

  /* Flags to enable virtual Terminal processing */
#define VT_FLAG_OUT ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define VT_FLAG_IN  ENABLE_VIRTUAL_TERMINAL_INPUT

  /* Default flags for input/output modes */
#define CONMODE_IN_DEFAULT (ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)
#define CONMODE_OUT_DEFAULT (ENABLE_PROCESSED_OUTPUT | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_LVB_GRID_WORLDWIDE)

  /* Flags to reset from RAW/CBREAK */
#define CONMODE_NORAW    (ENABLE_PROCESSED_INPUT|ENABLE_LINE_INPUT)
#define CONMODE_NOCBREAK (ENABLE_LINE_INPUT)


#if defined(USE_TERM_DRIVER) && defined(USE_WIN32CON_DRIVER)
extern NCURSES_EXPORT_VAR(TERM_DRIVER) _nc_WIN_DRIVER;
#endif

#define CON_NUMPAIRS 64
typedef struct {
    BOOL initialized;
    BOOL buffered;
    BOOL window_only;
    BOOL progMode;
    BOOL isMinTTY;
    BOOL isTermInfoConsole;
    HANDLE out;
    HANDLE inp;
    HANDLE hdl;
    HANDLE lastOut;
    int numButtons;
    LPDWORD ansi_map;
    LPDWORD map;
    LPDWORD rmap;
    WORD pairs[CON_NUMPAIRS];
    COORD origin;
    CHAR_INFO *save_screen;
    COORD save_size;
    SMALL_RECT save_region;
    CONSOLE_SCREEN_BUFFER_INFO SBI;
    CONSOLE_SCREEN_BUFFER_INFO save_SBI;
    CONSOLE_CURSOR_INFO save_CI;
    TTY originalMode;
} ConsoleInfo;

extern NCURSES_EXPORT_VAR(ConsoleInfo) _nc_CONSOLE;
#define WINCONSOLE _nc_CONSOLE

#define TypeAlloca(type,count)(type*) _alloca(sizeof(type)*(size_t)(count))

#endif /* EVENTLIST_2nd */

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 || _WIN64 */

#endif /* NC_WIN32_H */
