#include "libc/nt/ntdllimport.h"
.ntimp	RtlDeleteCriticalSection,RtlDeleteCriticalSection

#ifdef __x86_64__
	.text.windows
RtlDeleteCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDeleteCriticalSection(%rip)
	leave
	ret
	.endfn	RtlDeleteCriticalSection,globl
	.previous
#endif
