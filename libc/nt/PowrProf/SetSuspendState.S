#include "libc/nt/codegen.h"
.imp	PowrProf,__imp_SetSuspendState,SetSuspendState,0

#ifdef __x86_64__
	.text.windows
SetSuspendState:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetSuspendState(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetSuspendState,globl
	.previous
#endif
