/* Test the access rights of a file.
   Copyright (C) 2019-2020 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <fcntl.h>
#include <io.h>

int
access (const char *file, int mode)
{
  if ((mode & X_OK) != 0)
    mode = (mode & ~X_OK) | R_OK;
  return _access (file, mode);
}
