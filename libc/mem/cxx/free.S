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

/	Frees memory the C++ way.
/
/	\param	%rdi is pointer, or NULL for no-op
/	\param	%rsi is ignored
/	\param	%rdx is ignored
_ZdlPvSt11align_val_tRKSt9nothrow_t:
/	operator delete(void*, std::align_val_t, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvSt11align_val_tRKSt9nothrow_t,weak
_ZdaPvSt11align_val_tRKSt9nothrow_t:
/	operator delete[](void*, std::align_val_t, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvSt11align_val_tRKSt9nothrow_t,weak
_ZdlPvRKSt9nothrow_t:
/	operator delete(void*, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvRKSt9nothrow_t,weak
_ZdaPvRKSt9nothrow_t:
/	operator delete[](void*, std::nothrow_t const&)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvRKSt9nothrow_t,weak
_ZdlPvmSt11align_val_t:
/	operator delete(void*, unsigned long, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvmSt11align_val_t,weak
_ZdaPvmSt11align_val_t:
/	operator delete[](void*, unsigned long, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvmSt11align_val_t,weak
_ZdlPvSt11align_val_t:
/	operator delete(void*, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvSt11align_val_t,weak
_ZdaPvSt11align_val_t:
/	operator delete[](void*, std::align_val_t)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvSt11align_val_t,weak
_ZdaPvm:	
/	operator delete[](void*, unsigned long):
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPvm,weak
_ZdlPvm:
/	operator delete(void*, unsigned long)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdlPvm,weak
_ZdaPv:
/	operator delete[](void*)
	nop
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	_ZdaPv,weak
_ZdlPv:
/	operator delete(void*)
	jmp	*hook$free(%rip)
	.endfn	_ZdlPv,weak
