#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryObject,NtQueryObject

	.text.windows
	.ftrace1
NtQueryObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryObject(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryObject,globl
	.previous
