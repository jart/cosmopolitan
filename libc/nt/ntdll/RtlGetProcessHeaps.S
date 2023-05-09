#include "libc/nt/ntdllimport.h"
.ntimp	RtlGetProcessHeaps,RtlGetProcessHeaps

	.text.windows
RtlGetProcessHeaps:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlGetProcessHeaps(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlGetProcessHeaps,globl
	.previous
