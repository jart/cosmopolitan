#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetSystemTimeAdjustment,GetSystemTimeAdjustment,0

#ifdef __x86_64__
	.text.windows
GetSystemTimeAdjustment:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemTimeAdjustment(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemTimeAdjustment,globl
	.previous
#endif
