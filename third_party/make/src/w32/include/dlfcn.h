/* dlfcn.h replacement for MS-Windows build.
Copyright (C) 2013-2020 Free Software Foundation, Inc.
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

#ifndef DLFCN_H
#define DLFCN_H

#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 4

extern void *dlopen (const char *, int);
extern void *dlsym (void *, const char *);
extern char *dlerror (void);
extern int   dlclose (void *);

#endif  /* DLFCN_H */
