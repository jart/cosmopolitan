#include "libc/nt/ntdllimport.h"
.ntimp	RtlSizeHeap,RtlSizeHeap

	.text.windows
RtlSizeHeap:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlSizeHeap(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlSizeHeap,globl
	.previous
