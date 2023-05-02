#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetWaitableTimer,SetWaitableTimer,0

#ifdef __x86_64__
	.text.windows
SetWaitableTimer:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWaitableTimer(%rip),%rax
	jmp	__sysv2nt6
	.endfn	SetWaitableTimer,globl
	.previous
#endif
