#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetThreadPriorityBoost,SetThreadPriorityBoost,0

#ifdef __x86_64__
	.text.windows
SetThreadPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadPriorityBoost,globl
	.previous
#endif
