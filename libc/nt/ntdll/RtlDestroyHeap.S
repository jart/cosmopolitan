#include "libc/nt/ntdllimport.h"
.ntimp	RtlDestroyHeap,RtlDestroyHeap

	.text.windows
	.ftrace1
RtlDestroyHeap:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDestroyHeap(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlDestroyHeap,globl
	.previous
