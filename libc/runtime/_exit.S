/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/runtime/internal.h"
#include "libc/macros.h"
.privileged
.yoink	__FILE__

/	Terminates process, ignoring destructors and atexit() handlers.
/
/	Normally exit() or quick_exit() is better. This won't even flush
/	stdio streams. Sometimes that makes sense, like after fork().
/
/	@param	edi is exit code ∈ [0,256)
/	@note	_exit() is same thing
/	@asyncsignalsafe
_Exit:	orl	$RUNSTATE_TERMINATE,g_runstate(%rip)
#if SupportsWindows()
	testb	IsWindows()
	jz	1f
	movzbl	%dil,%ecx		# %ERRORLEVEL% is limitless
4:	call	*__imp_ExitProcess(%rip)
	jmp	4b
0:	int3				# @see setjmp() in WinMain()
#endif
1:	mov	__NR_exit(%rip),%eax
	syscall
	cli
	lidt	3f
2:	hlt
	jmp	2b
3:	.quad	0
	.endfn	_Exit,globl,protected
	.hidden	__NR_exit
	.hidden	g_runstate
