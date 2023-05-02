#include "libc/nt/ntdllimport.h"
.ntimp	LdrGetProcedureAddress,LdrGetProcedureAddress

#ifdef __x86_64__
	.text.windows
LdrGetProcedureAddress:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LdrGetProcedureAddress(%rip),%rax
	jmp	__sysv2nt
	.endfn	LdrGetProcedureAddress,globl
	.previous
#endif
