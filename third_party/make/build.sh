#!/bin/sh
# Shell script to build GNU Make in the absence of any 'make' program.

# Copyright (C) 1993-2020 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
#
# GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

# Get configure-generated values
. ./build.cfg

: ${OUTDIR:=.}
OUTLIB="$OUTDIR/lib"

# Directory to find libraries in for '-lXXX'.
libdir=$exec_prefix/lib
# Directory to search by default for included makefiles.
includedir=$prefix/include

localedir=$prefix/share/locale

defines="-DLOCALEDIR=\"$localedir\" -DLIBDIR=\"$libdir\" -DINCLUDEDIR=\"$includedir\""

# Look up a make variable value.
# It can handle simple recursion where variables are separate words.
# Print the value to stdout.
get_mk_var ()
{
  file=$1
  var=$2

  val=
  v=$(sed -e :a -e '/\\$/N; s/\\\n//; ta' "$file" | sed -n "s=^ *$var *\= *==p")
  for w in $v; do
    case $w in
      (\$[\(\{]*[\)\}]) w=${w#\$[\(\{]}; w=$(get_mk_var "$file" "${w%[\)\}]}") ;;
    esac
    val="${val:+$val }$w"
  done

  printf '%s\n' "$val"
}

# Compile source files.  Object files are put into $objs.
compile ()
{
  objs=
  for ofile in "$@"; do
    file="${ofile%.$OBJEXT}.c"
    echo "compiling $file..."
    of="$OUTDIR/$ofile"
    mkdir -p "${of%/*}"
    $CC $cflags $CPPFLAGS $CFLAGS -c -o "$of" "$top_srcdir/$file"
    objs="${objs:+$objs }$of"
  done
}

# Use config.status to convert a .in file.  Output file is put into $out.
# $out will be empty if no conversion was needed.
convert ()
{
  out=
  base=$1
  var="GENERATE_$(echo $base | tr 'a-z./+' A-Z__X)"

  # Is this file disabled?
  grep "${var}_FALSE\"]=\"\"" config.status >/dev/null && return

  # Not disabled, so create it
  in="$top_srcdir/lib/$(echo ${base%.*}.in.${base##*.} | tr / _)"
  out="$OUTLIB/$base"
  mkdir -p "${out%/*}"

  # First perform the normal replacements, using config.status
  sed -e 's|@GUARD_PREFIX@|GL|g' \
      -e 's/@GNULIB_UNISTD_H_GETOPT@/0/g' \
      "$in" > "${out}_"
  ./config.status --file "${out}__:${out}_"
  int="${out}__"

  # Then see if there any files we need to include.  Unfortunately there's no
  # algorithmic conversion so we just have to hard-code it.
  incls=$(sed -n 's/.*definitions* of \(_[^ $]*\).*/\1/p' "$in")

  for inc in $incls; do
    case $inc in
      (_GL_FUNCDECL_RPL) fn=$(get_mk_var lib/Makefile CXXDEFS_H) ;;
      (_GL_ARG_NONNULL)  fn=$(get_mk_var lib/Makefile ARG_NONNULL_H) ;;
      (_GL_WARN_ON_USE)  fn=$(get_mk_var lib/Makefile WARN_ON_USE_H) ;;
      (_Noreturn)        fn=$(get_mk_var lib/Makefile _NORETURN_H) ;;
      (*) echo "Unknown file replacement: $inc"; exit 1 ;;
    esac

    fn="$top_srcdir/lib/${fn##*/}"
    [ -f "$fn" ] || { echo "Missing file: $fn"; exit 1; }

    sed "/definitions* of $inc/r $fn" "$int" > "${int}_"
    int=${int}_
  done

  # Done!
  mv "$int" "$out"
}

# Get source files provided from gnulib and convert to object files
LIBOBJS=
for lo in $( (get_mk_var lib/Makefile libgnu_a_OBJECTS; get_mk_var lib/Makefile libgnu_a_LIBADD) | sed "s=\$[\(\{]OBJEXT[\)\}]=$OBJEXT=g"); do
  LIBOBJS="${LIBOBJS:+$LIBOBJS }lib/$lo"
done

# Get object files from the Makefile
OBJS=$(get_mk_var Makefile make_OBJECTS | sed "s=\$[\(\{]OBJEXT[\)\}]=$OBJEXT=g")

# Exit as soon as any command fails.
set -e

# Generate gnulib header files that would normally be created by make
for b in $(get_mk_var lib/Makefile BUILT_SOURCES); do
    convert $b
done

# Build the gnulib library
cflags="$DEFS -I$OUTLIB -Ilib -I$top_srcdir/lib -I$OUTDIR/src -Isrc -I$top_srcdir/src"
compile $LIBOBJS

echo "creating libgnu.a..."
$AR $ARFLAGS "$OUTLIB"/libgnu.a $objs

# Compile the source files into those objects.
cflags="$DEFS $defines -I$OUTDIR/src -Isrc -I$top_srcdir/src -I$OUTLIB -Ilib -I$top_srcdir/lib"
compile $OBJS

# Link all the objects together.
echo "linking make..."
$CC $CFLAGS $LDFLAGS -L"$OUTLIB" $objs -lgnu $LOADLIBES -o "$OUTDIR/makenew$EXEEXT"
mv -f "$OUTDIR/makenew$EXEEXT" "$OUTDIR/make$EXEEXT"

echo done.
