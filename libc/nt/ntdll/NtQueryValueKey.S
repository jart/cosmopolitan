#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryValueKey,NtQueryValueKey

	.text.windows
	.ftrace1
NtQueryValueKey:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtQueryValueKey(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryValueKey,globl
	.previous
