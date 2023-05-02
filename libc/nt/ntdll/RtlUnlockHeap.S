#include "libc/nt/ntdllimport.h"
.ntimp	RtlUnlockHeap,RtlUnlockHeap

#ifdef __x86_64__
	.text.windows
RtlUnlockHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlUnlockHeap(%rip)
	leave
	ret
	.endfn	RtlUnlockHeap,globl
	.previous
#endif
