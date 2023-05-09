#include "libc/nt/ntdllimport.h"
.ntimp	RtlDeleteCriticalSection,RtlDeleteCriticalSection

	.text.windows
RtlDeleteCriticalSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDeleteCriticalSection(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlDeleteCriticalSection,globl
	.previous
