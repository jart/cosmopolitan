#include "libc/nt/ntdllimport.h"
.ntimp	NtOpenSymbolicLinkObject,NtOpenSymbolicLinkObject

	.text.windows
	.ftrace1
NtOpenSymbolicLinkObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtOpenSymbolicLinkObject(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtOpenSymbolicLinkObject,globl
	.previous
