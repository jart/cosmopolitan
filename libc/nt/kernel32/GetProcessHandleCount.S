#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessHandleCount,GetProcessHandleCount,0

	.text.windows
GetProcessHandleCount:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessHandleCount(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessHandleCount,globl
	.previous
