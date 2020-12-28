/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"
.source	__FILE__

/	Allocates memory the C++ way.
/
/	\param	%rdi is bytes to allocate
/	\param	%rsi is ignored
/	\return	new memory or NULL on OOM
_ZnamRKSt9nothrow_t:
/	operator new[](unsigned long, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZnamRKSt9nothrow_t,weak
_ZnwmRKSt9nothrow_t:
/	operator new(unsigned long, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZnwmRKSt9nothrow_t,weak
_Znam:
/	operator new[](unsigned long)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_Znam,weak
_Znwm:
/	operator new(unsigned long)
	test	%rdi,%rdi
	jne	1f
	mov	$1,%edi
1:	jmp	*hook$malloc(%rip)
	.endfn	_Znwm,weak
