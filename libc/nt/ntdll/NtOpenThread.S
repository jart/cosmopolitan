#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenThread,NtOpenThread

	.text.windows
	.ftrace1
NtOpenThread:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenThread(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenThread,globl
	.previous
