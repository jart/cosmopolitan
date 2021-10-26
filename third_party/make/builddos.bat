@echo off
rem Copyright (C) 1998-2020 Free Software Foundation, Inc.
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

echo Building Make for MSDOS with DJGPP

rem The SmallEnv trick protects against too small environment block,
rem in which case the values will be truncated and the whole thing
rem goes awry.  COMMAND.COM will say "Out of environment space", but
rem many people don't care, so we force them to care by refusing to go.

rem Where is the srcdir?
set XSRC=.
if not "%XSRC%"=="." goto SmallEnv
if "%1%"=="" goto SrcDone
if "%1%"=="." goto SrcDone
set XSRC=%1

if not "%XSRC%"=="%1" goto SmallEnv

:SrcDone

if not exist src mkdir src
if not exist lib mkdir lib

copy /Y %XSRC%\src\configh.dos .\src\config.h

copy /Y %XSRC%\lib\glob.in.h .\lib\glob.h
copy /Y %XSRC%\lib\fnmatch.in.h .\lib\fnmatch.h

rem Echo ON so they will see what is going on.
@echo on
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/commands.c -o commands.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/output.c -o output.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/job.c -o job.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/dir.c -o dir.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/file.c -o file.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/misc.c -o misc.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/main.c -o main.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -DINCLUDEDIR=\"c:/djgpp/include\" -O2 -g %XSRC%/src/read.c -o read.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -DLIBDIR=\"c:/djgpp/lib\" -O2 -g %XSRC%/src/remake.c -o remake.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/rule.c -o rule.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/implicit.c -o implicit.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/default.c -o default.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/variable.c -o variable.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/expand.c -o eyxpand.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/function.c -o function.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/vpath.c -o vpath.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/hash.c -o hash.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/strcache.c -o strcache.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/version.c -o version.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/ar.c -o ar.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/arscan.c -o arscan.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/signame.c -o signame.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/remote-stub.c -o remote-stub.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/getopt.c -o getopt.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/src/getopt1.c -o getopt1.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/lib/glob.c -o lib/glob.o
gcc -c -I./src -I%XSRC%/src -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/lib/fnmatch.c -o lib/fnmatch.o
@echo off
echo commands.o > respf.$$$
for %%f in (job output dir file misc main read remake rule implicit default variable) do echo %%f.o >> respf.$$$
for %%f in (expand function vpath hash strcache version ar arscan signame remote-stub getopt getopt1) do echo %%f.o >> respf.$$$
for %%f in (lib\glob lib\fnmatch) do echo %%f.o >> respf.$$$
rem gcc  -c -I./src -I%XSRC% -I./lib -I%XSRC%/lib -DHAVE_CONFIG_H -O2 -g %XSRC%/guile.c -o guile.o
rem echo guile.o >> respf.$$$
@echo Linking...
@echo on
gcc -o make.exe @respf.$$$
@echo off
if not exist make.exe echo Make.exe build failed...
if exist make.exe echo make.exe is now built!
if exist make.exe del respf.$$$
if exist make.exe copy /Y Basic.mk Makefile
goto End

:SmallEnv
echo Your environment is too small.  Please enlarge it and run me again.

:End
set XRSC=
@echo on
