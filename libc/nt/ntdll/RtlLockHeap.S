#include "libc/nt/ntdllimport.h"
.ntimp	RtlLockHeap,RtlLockHeap

#ifdef __x86_64__
	.text.windows
RtlLockHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlLockHeap(%rip)
	leave
	ret
	.endfn	RtlLockHeap,globl
	.previous
#endif
