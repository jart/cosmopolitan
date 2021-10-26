/* Record version and build host architecture for GNU make.
Copyright (C) 1988-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because makeint.h was found in $srcdir).  */
#include <config.h>

#ifndef MAKE_HOST
# define MAKE_HOST "unknown"
#endif

const char *version_string = VERSION;
const char *make_host = MAKE_HOST;

/*
  Local variables:
  version-control: never
  End:
 */
