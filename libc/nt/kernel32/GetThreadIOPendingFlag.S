#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetThreadIOPendingFlag,GetThreadIOPendingFlag,0

	.text.windows
GetThreadIOPendingFlag:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetThreadIOPendingFlag(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetThreadIOPendingFlag,globl
	.previous
