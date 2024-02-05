#include "libc/nt/ntdllimport.h"
.ntimp	RtlLeaveCriticalSection,RtlLeaveCriticalSection

	.text.windows
	.ftrace1
RtlLeaveCriticalSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlLeaveCriticalSection(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlLeaveCriticalSection,globl
	.previous
