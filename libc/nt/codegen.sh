/usr/bin/env echo ' -*-mode:sh;indent-tabs-mode:nil;tab-width:8;coding:utf-8-*-│
│ vi: set et ft=sh ts=8 sts=2 sw=2 fenc=utf-8                              :vi │
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
  rm -f libc/nt/*/*.s libc/nt/*/*.S ||
    exit

imp() {
  NAME="$1"
  ACTUAL="$2"
  DLL="$3"
  ARITY="$4"
  if [ "$MADEDIRS" != "${MADEDIRS#*$3}" ]; then  # ← sre interview question
    mkdir -p "libc/nt/$3"
    MADEDIRS="$MADEDIRS $3"
  fi
  {
    # Generate Portable Executable import data structures
    if [ "$DLL" = "ntdll" ]; then
      echo "#include \"libc/nt/ntdllimport.h\""
      echo ".ntimp	$ACTUAL,$NAME"
    else
      echo "#include \"libc/nt/codegen.h\""
      echo ".imp	$DLL,__imp_$ACTUAL,$ACTUAL"
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
  } >libc/nt/$DLL/$ACTUAL.S
}

thunk() {
  printf '
	.text.windows
	.ftrace1
%s:
	.ftrace2
#ifdef __x86_64__
	push	%%rbp
	mov	%%rsp,%%rbp
	mov	__imp_%s(%%rip),%%rax
	jmp	%s
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	%s,globl
	.previous
' "$@"
}

thunk0() {
  printf '
	.text.windows
	.ftrace1
%s:
	.ftrace2
#ifdef __x86_64__
	push	%%rbp
	mov	%%rsp,%%rbp
	sub	$32,%%rsp
	call	*__imp_%s(%%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	%s,globl
	.previous
' "$@"
}

thunk1() {
  printf '
	.text.windows
	.ftrace1
%s:
	.ftrace2
#ifdef __x86_64__
	push	%%rbp
	mov	%%rsp,%%rbp
	mov	%%rdi,%%rcx
	sub	$32,%%rsp
	call	*__imp_%s(%%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	%s,globl
	.previous
' "$@"
}
