/****************************************************************************
 * Copyright 2020 Thomas E. Dickey                                          *
 * Copyright 2007-2008,2017 Free Software Foundation, Inc.                  *
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

/*
**	lib_key_name.c
**
**	The routine key_name().
**
*/

#include "curses.priv.h"

MODULE_ID("$Id: lib_key_name.c,v 1.5 2020/02/02 23:34:34 tom Exp $")

#define MyData _nc_globals.key_name

NCURSES_EXPORT(NCURSES_CONST char *)
key_name(wchar_t c)
{
    cchar_t my_cchar;
    wchar_t *my_wchars;
    size_t len;

    memset(&my_cchar, 0, sizeof(my_cchar));
    my_cchar.chars[0] = c;
    my_cchar.chars[1] = L'\0';

    my_wchars = wunctrl(&my_cchar);
    len = wcstombs(MyData, my_wchars, sizeof(MyData) - 1);
    if (isEILSEQ(len) || (len == 0)) {
	return 0;
    }

    MyData[len] = '\0';
    return MyData;
}
