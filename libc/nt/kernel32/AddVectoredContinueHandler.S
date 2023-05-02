#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AddVectoredContinueHandler,AddVectoredContinueHandler,0

#ifdef __x86_64__
	.text.windows
AddVectoredContinueHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredContinueHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	AddVectoredContinueHandler,globl
	.previous
#endif
