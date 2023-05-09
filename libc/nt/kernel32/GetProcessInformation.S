#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessInformation,GetProcessInformation,0

	.text.windows
GetProcessInformation:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessInformation(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessInformation,globl
	.previous
