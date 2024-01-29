#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateTimer,NtCreateTimer

	.text.windows
	.ftrace1
NtCreateTimer:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateTimer(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateTimer,globl
	.previous
