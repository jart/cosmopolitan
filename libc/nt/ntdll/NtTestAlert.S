#include "libc/nt/ntdllimport.h"
.ntimp	NtTestAlert,NtTestAlert

	.text.windows
	.ftrace1
NtTestAlert:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_NtTestAlert(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	NtTestAlert,globl
	.previous
