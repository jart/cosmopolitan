/* Program name management.
   Copyright (C) 2016-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* clang-format off */
#include "third_party/make/config.h"
#include "third_party/make/getprogname.h"
#include "libc/intrin/safemacros.internal.h"
#include "third_party/make/dirname.h"

char const *
getprogname (void)
{
  return firstnonnull(program_invocation_short_name, "unknown");
}

/*
 * Hey Emacs!
 * Local Variables:
 * coding: utf-8
 * End:
 */
