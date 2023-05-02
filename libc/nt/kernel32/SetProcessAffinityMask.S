#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetProcessAffinityMask,SetProcessAffinityMask,1347

#ifdef __x86_64__
	.text.windows
SetProcessAffinityMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetProcessAffinityMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetProcessAffinityMask,globl
	.previous
#endif
