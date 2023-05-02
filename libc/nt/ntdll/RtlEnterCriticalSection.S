#include "libc/nt/ntdllimport.h"
.ntimp	RtlEnterCriticalSection,RtlEnterCriticalSection

#ifdef __x86_64__
	.text.windows
RtlEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlEnterCriticalSection(%rip)
	leave
	ret
	.endfn	RtlEnterCriticalSection,globl
	.previous
#endif
