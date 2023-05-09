#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetProcessTimes,GetProcessTimes,0

	.text.windows
GetProcessTimes:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessTimes(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetProcessTimes,globl
	.previous
