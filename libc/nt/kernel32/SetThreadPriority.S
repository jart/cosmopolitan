#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetThreadPriority,SetThreadPriority,0

#ifdef __x86_64__
	.text.windows
SetThreadPriority:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadPriority(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadPriority,globl
	.previous
#endif
