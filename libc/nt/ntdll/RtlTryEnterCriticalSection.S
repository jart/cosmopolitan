#include "libc/nt/ntdllimport.h"
.ntimp	RtlTryEnterCriticalSection,RtlTryEnterCriticalSection

	.text.windows
	.ftrace1
RtlTryEnterCriticalSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlTryEnterCriticalSection(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlTryEnterCriticalSection,globl
	.previous
