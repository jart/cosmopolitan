#include "libc/nt/codegen.h"
.imp	kernel32,__imp_InitializeCriticalSectionAndSpinCount,InitializeCriticalSectionAndSpinCount,0

#ifdef __x86_64__
	.text.windows
InitializeCriticalSectionAndSpinCount:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InitializeCriticalSectionAndSpinCount(%rip),%rax
	jmp	__sysv2nt
	.endfn	InitializeCriticalSectionAndSpinCount,globl
	.previous
#endif
