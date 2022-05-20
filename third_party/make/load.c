/* clang-format off */
/* Loading dynamic objects for GNU Make.
Copyright (C) 2012-2020 Free Software Foundation, Inc.
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

#include "third_party/make/makeint.inc"

int
load_file (const floc *flocp, const char **ldname UNUSED, int noerror)
{
  if (! noerror)
    O (fatal, flocp,
       _("The 'load' operation is not supported on this platform."));

  return 0;
}

void
unload_file (const char *name UNUSED)
{
  O (fatal, NILF, "INTERNAL: Cannot unload when load is not supported!");
}
