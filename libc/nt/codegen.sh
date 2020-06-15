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
MADEDIRS=

mkdir -p libc/nt/kernel32 &&
  touch libc/nt/kernel32/boop.s &&
  rm -f libc/nt/*/*.s ||
    exit

imp() {
  NAME="$1"
  ACTUAL="$2"
  DLL="$3"
  HINT="$4"
  ARITY="$5"
  if [ "$MADEDIRS" != "${MADEDIRS#*$3}" ]; then  # ← sre interview question
    mkdir -p "libc/nt/$3"
    MADEDIRS="$MADEDIRS $3"
  fi
  {
    # Generate Portable Executable import data structures
    if [ "$DLL" = "ntdll" ]; then
      echo ".include \"o/libc/nt/ntdllimport.inc\""
      echo ".ntimp	$ACTUAL"
    else
      echo ".include \"o/libc/nt/codegen.inc\""
      echo ".imp	$DLL,__imp_$ACTUAL,$ACTUAL,$HINT"
    fi

    # Generate System Five ABI translating thunks
    if [ -n "$NAME" ]; then
      case "$ARITY" in
        0)     thunk0 "$NAME" "$ACTUAL" "$NAME" ;;
        1)     thunk1 "$NAME" "$ACTUAL" "$NAME" ;;
        2|3|4) thunk "$NAME" "$ACTUAL" __sysv2nt "$NAME" ;;
        5|6)   thunk "$NAME" "$ACTUAL" __sysv2nt6 "$NAME" ;;
        7|8)   thunk "$NAME" "$ACTUAL" __sysv2nt8 "$NAME" ;;
        9|10)  thunk "$NAME" "$ACTUAL" __sysv2nt10 "$NAME" ;;
        11|12) thunk "$NAME" "$ACTUAL" __sysv2nt12 "$NAME" ;;
        13|14) thunk "$NAME" "$ACTUAL" __sysv2nt14 "$NAME" ;;
      esac
    fi
  } >libc/nt/$DLL/$ACTUAL.s
}

thunk() {
  printf '
	.text.windows
%s:
	push	%%rbp
	mov	%%rsp,%%rbp
	.profilable
	mov	__imp_%s(%%rip),%%rax
	jmp	%s
	.endfn	%s,globl
	.previous
' "$@"
}

thunk0() {
  printf '
	.text.windows
%s:
	push	%%rbp
	mov	%%rsp,%%rbp
	.profilable
	sub	$32,%%rsp
	call	*__imp_%s(%%rip)
	leave
	ret
	.endfn	%s,globl
	.previous
' "$@"
}

thunk1() {
  printf '
	.text.windows
%s:
	push	%%rbp
	mov	%%rsp,%%rbp
	.profilable
	mov	%%rdi,%%rcx
	sub	$32,%%rsp
	call	*__imp_%s(%%rip)
	leave
	ret
	.endfn	%s,globl
	.previous
' "$@"
}
