#include "libc/nt/ntdllimport.h"
.ntimp	NtQuerySymbolicLinkObject,NtQuerySymbolicLinkObject

	.text.windows
	.ftrace1
NtQuerySymbolicLinkObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQuerySymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQuerySymbolicLinkObject,globl
	.previous
