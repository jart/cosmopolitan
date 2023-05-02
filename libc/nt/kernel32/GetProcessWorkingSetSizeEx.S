#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessWorkingSetSizeEx,GetProcessWorkingSetSizeEx,0

#ifdef __x86_64__
	.text.windows
GetProcessWorkingSetSizeEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessWorkingSetSizeEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessWorkingSetSizeEx,globl
	.previous
#endif
