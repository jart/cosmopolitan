#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadIOPendingFlag,GetThreadIOPendingFlag,0

#ifdef __x86_64__
	.text.windows
GetThreadIOPendingFlag:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadIOPendingFlag(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetThreadIOPendingFlag,globl
	.previous
#endif
