#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetThreadAffinityMask,SetThreadAffinityMask,1369

#ifdef __x86_64__
	.text.windows
SetThreadAffinityMask:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetThreadAffinityMask(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetThreadAffinityMask,globl
	.previous
#endif
