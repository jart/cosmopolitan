#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateIoCompletion,NtCreateIoCompletion

	.text.windows
	.ftrace1
NtCreateIoCompletion:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateIoCompletion(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateIoCompletion,globl
	.previous
