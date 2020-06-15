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
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/nr.h"
#include "libc/macros.h"
.real
.yoink	__FILE__

/	Terminates program abnormally.
/
/	This function first tries to trigger your SIGABRT handler. If
/	there isn't one or execution resumes, then abort() terminates
/	the program using an escalating variety methods of increasing
/	brutality.
/
/	@forcealignargpointer
/	@asyncsignalsafe
/	@noreturn
abort:	push	%rbp
	mov	%rsp,%rbp
	and	$-16,%rsp
	sub	$16,%rsp
	orl	$RUNSTATE_BROKEN,g_runstate(%rip)
	testb	IsWindows()
	jnz	2f
	mov	SIG_SETMASK,%edi
	mov	%rsp,%rsi
	push	$0xffffffffffffffdf		# all bits blocked but SIGABRT
	push	$0xffffffffffffffff		# assumes von neum. arithmetic
	pop	8(%rsi)
	pop	(%rsi)
	xor	%edx,%edx			# don't care about old sigmask
	pushpop	4*4,%r10			# sizeof(sigset_t) for systemd
	mov	__NR_sigprocmask,%eax		# sigprocmask$sysv is hookable
	syscall
	mov	__NR_getpid,%eax
	syscall
	mov	%eax,%edi
	mov	SIGABRT,%esi
	mov	__NR_kill,%eax
	syscall					# avoid hook and less bt noise
2:	mov	$134,%edi			# exit(128+SIGABRT) [bash-ism]
	call	_Exit
	.endfn	abort,globl,protected
	.hidden	g_runstate
