#include "libc/nt/ntdllimport.h"
.ntimp	RtlDestroyProcessParameters,RtlDestroyProcessParameters

	.text.windows
	.ftrace1
RtlDestroyProcessParameters:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDestroyProcessParameters(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	RtlDestroyProcessParameters,globl
	.previous
