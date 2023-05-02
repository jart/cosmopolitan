#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemTimes,GetSystemTimes,0

#ifdef __x86_64__
	.text.windows
GetSystemTimes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemTimes(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemTimes,globl
	.previous
#endif
