/*
*   $Id: main.h 443 2006-05-30 04:37:13Z darren $
*
*   Copyright (c) 1998-2002, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to main.c
*/
#ifndef _MAIN_H
#define _MAIN_H

/*
*   INCLUDE FILES
*/
#include "third_party/ctags/general.h"  /* must always come first */

#include "libc/calls/calls.h"
#include "libc/stdio/dprintf.h"
#include "libc/calls/weirdtypes.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "third_party/musl/tempnam.h"

#include "third_party/ctags/vstring.h"

/*
*   FUNCTION PROTOTYPES
*/
extern void addTotals (const unsigned int files, const long unsigned int lines, const long unsigned int bytes);
extern boolean isDestinationStdout (void);
extern int main (int argc, char **argv);

#endif  /* _MAIN_H */

/* vi:set tabstop=4 shiftwidth=4: */
