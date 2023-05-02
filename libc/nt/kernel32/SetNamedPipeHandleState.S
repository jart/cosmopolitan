#include "libc/nt/codegen.h"
.imp	kernel32,__imp_SetNamedPipeHandleState,SetNamedPipeHandleState,0

#ifdef __x86_64__
	.text.windows
SetNamedPipeHandleState:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetNamedPipeHandleState(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetNamedPipeHandleState,globl
	.previous
#endif
