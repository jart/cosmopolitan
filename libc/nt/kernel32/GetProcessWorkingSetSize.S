#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessWorkingSetSize,GetProcessWorkingSetSize,708

#ifdef __x86_64__
	.text.windows
GetProcessWorkingSetSize:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessWorkingSetSize(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessWorkingSetSize,globl
	.previous
#endif
