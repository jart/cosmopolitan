#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetCriticalSectionSpinCount,SetCriticalSectionSpinCount,0

#ifdef __x86_64__
	.text.windows
SetCriticalSectionSpinCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetCriticalSectionSpinCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetCriticalSectionSpinCount,globl
	.previous
#endif
