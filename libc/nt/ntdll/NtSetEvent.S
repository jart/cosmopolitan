#include "libc/nt/ntdllimport.h"
.ntimp	NtSetEvent,NtSetEvent

	.text.windows
	.ftrace1
NtSetEvent:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtSetEvent(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtSetEvent,globl
	.previous
