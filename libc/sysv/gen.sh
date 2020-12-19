/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
mkdir -p $dir
rm -f $dir/*.s $dir/*.S

scall() {
  {
    echo ".include \"o/libc/sysv/macros.internal.inc\""
    echo ".scall" "$@"
  } >"$dir/${1/$/-}.s"
}

syscon() {
  {
    echo ".include \"libc/sysv/consts/syscon.inc\""
    echo ".syscon" "$@"
  } >"$dir/${2/$/-}.s"
}

sysstr() {
  {
    echo ".include \"o/libc/sysv/macros.internal.inc\""
    echo ".sysstr $1 \"$2\""
  } >"$dir/${1/$/-}.s"
}

errfun() {
  NAME="$1"
  ERRNO="$2"
  {
    printf '#include "libc/macros.h"\n.text.unlikely\n\n'
    printf '%s:' "$NAME"
    if [ "${#NAME}" -gt 6 ]; then
      printf '\n'
    fi
    printf '	.leafprologue
	.profilable
	mov	%s(%%rip),%%eax
	mov	%%eax,errno(%%rip)
	push	$-1
	pop	%%rax
	.leafepilogue
	.endfn	%s,globl,hidden
' "$ERRNO" "$NAME"
  } >"$dir/${1/$/-}.S"
}
