#include "libc/nt/ntdllimport.h"
.ntimp	RtlTryEnterCriticalSection,RtlTryEnterCriticalSection

#ifdef __x86_64__
	.text.windows
RtlTryEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlTryEnterCriticalSection(%rip)
	leave
	ret
	.endfn	RtlTryEnterCriticalSection,globl
	.previous
#endif
