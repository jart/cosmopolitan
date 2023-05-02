#include "libc/nt/ntdllimport.h"
.ntimp	RtlLeaveCriticalSection,RtlLeaveCriticalSection

#ifdef __x86_64__
	.text.windows
RtlLeaveCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlLeaveCriticalSection(%rip)
	leave
	ret
	.endfn	RtlLeaveCriticalSection,globl
	.previous
#endif
