#include "libc/nt/ntdllimport.h"
.ntimp	NtCreateSection,NtCreateSection

	.text.windows
	.ftrace1
NtCreateSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtCreateSection(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtCreateSection,globl
	.previous
