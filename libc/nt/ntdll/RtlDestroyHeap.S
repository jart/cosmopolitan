#include "libc/nt/ntdllimport.h"
.ntimp	RtlDestroyHeap,RtlDestroyHeap

#ifdef __x86_64__
	.text.windows
RtlDestroyHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDestroyHeap(%rip)
	leave
	ret
	.endfn	RtlDestroyHeap,globl
	.previous
#endif
