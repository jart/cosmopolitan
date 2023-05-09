#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AddVectoredContinueHandler,AddVectoredContinueHandler,0

	.text.windows
AddVectoredContinueHandler:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredContinueHandler(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AddVectoredContinueHandler,globl
	.previous
