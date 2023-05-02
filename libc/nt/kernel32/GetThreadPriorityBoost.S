#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadPriorityBoost,GetThreadPriorityBoost,0

#ifdef __x86_64__
	.text.windows
GetThreadPriorityBoost:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadPriorityBoost(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetThreadPriorityBoost,globl
	.previous
#endif
