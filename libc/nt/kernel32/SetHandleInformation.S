#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetHandleInformation,SetHandleInformation,0

#ifdef __x86_64__
	.text.windows
SetHandleInformation:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetHandleInformation(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetHandleInformation,globl
	.previous
#endif
