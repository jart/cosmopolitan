/* test.h -- external interface to the conditional command code. */

/* Copyright (C) 1997-2021 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Bash is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Bash.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _TEST_H_
#define _TEST_H_

#include "stdc.h"

/* Values for the flags argument to binary_test */
#define TEST_PATMATCH	0x01
#define TEST_ARITHEXP	0x02
#define TEST_LOCALE	0x04
#define TEST_ARRAYEXP	0x08		/* array subscript expansion */

extern int test_unop PARAMS((char *));
extern int test_binop PARAMS((char *));

extern int unary_test PARAMS((char *, char *, int));
extern int binary_test PARAMS((char *, char *, char *, int));

extern int test_command PARAMS((int, char **));

#endif /* _TEST_H_ */
