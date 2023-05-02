#include "libc/nt/codegen.h"
.imp	kernel32,__imp_AddVectoredExceptionHandler,AddVectoredExceptionHandler,0

#ifdef __x86_64__
	.text.windows
AddVectoredExceptionHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AddVectoredExceptionHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	AddVectoredExceptionHandler,globl
	.previous
#endif
