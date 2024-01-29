#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateEvent,NtCreateEvent

	.text.windows
	.ftrace1
NtCreateEvent:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateEvent(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateEvent,globl
	.previous
