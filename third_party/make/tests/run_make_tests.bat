@echo off
rem Copyright (C) 2017-2020 Free Software Foundation, Inc.
rem This file is part of GNU Make.
rem
rem GNU Make is free software; you can redistribute it and/or modify it under
rem the terms of the GNU General Public License as published by the Free
rem Software Foundation; either version 3 of the License, or (at your option)
rem any later version.
rem
rem GNU Make is distributed in the hope that it will be useful, but WITHOUT
rem ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
rem FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for.
rem more details.
rem
rem You should have received a copy of the GNU General Public License along
rem with this program.  If not, see <http://www.gnu.org/licenses/>.

setlocal
cd "%~dp0"

perl -I. .\run_make_tests.pl %*
