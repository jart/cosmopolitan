$!
$! config_flags_pm.com  - Build config-flags.pm on VMS.
$!
$! Just good enough to run the self tests for now.
$!
$! Copyright (C) 2014-2020 Free Software Foundation, Inc.
$! This file is part of GNU Make.
$!
$! GNU Make is free software; you can redistribute it and/or modify it under
$! the terms of the GNU General Public License as published by the Free Software
$! Foundation; either version 3 of the License, or (at your option) any later
$! version.
$!
$! GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
$! WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
$! FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
$! details.
$!
$! You should have received a copy of the GNU General Public License along with
$! this program.  If not, see <http://www.gnu.org/licenses/>.
$!
$!
$ open/read cfpm_in config-flags.pm.in
$!
$ outfile = "sys$disk:[]config-flags.pm"
$!
$ cflags = "/include=([],[.lib]"
$!
$ create 'outfile'
$ open/append cfpm 'outfile'
$!
$cfpm_read_loop:
$   read cfpm_in/end=cfpm_read_loop_end line_in
$   line_in_len = f$length(line_in)
$   if f$locate("@", line_in) .lt. line_in_len
$   then
$       part1 = f$element(0, "@", line_in)
$       key = f$element(1, "@", line_in)
$       part2 = f$element(2, "@", line_in)
$       value = ""
$       if key .eqs. "CC" then value = "CC"
$       if key .eqs. "CPP" then value = "CPP"
$       if key .eqs. "CFLAGS" then value = cflags
$       if key .eqs. "GUILE_CFLAGS" then value = cflags
$       if key .eqs. "USE_SYSTEM_GLOB" then value = "no"
$       write cfpm part1, value, part2
$       goto cfpm_read_loop
$   endif
$   write cfpm line_in
$   goto cfpm_read_loop
$cfpm_read_loop_end:
$ close cfpm_in
$ close cfpm
$!
