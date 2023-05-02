#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessTimes,GetProcessTimes,0

#ifdef __x86_64__
	.text.windows
GetProcessTimes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessTimes(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetProcessTimes,globl
	.previous
#endif
