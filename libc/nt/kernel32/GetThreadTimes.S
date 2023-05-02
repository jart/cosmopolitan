#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadTimes,GetThreadTimes,0

#ifdef __x86_64__
	.text.windows
GetThreadTimes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadTimes(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetThreadTimes,globl
	.previous
#endif
