/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
