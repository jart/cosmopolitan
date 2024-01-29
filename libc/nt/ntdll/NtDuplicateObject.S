#include "libc/nt/ntdllimport.h"
.ntimp	NtDuplicateObject,NtDuplicateObject

	.text.windows
	.ftrace1
NtDuplicateObject:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtDuplicateObject(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtDuplicateObject,globl
	.previous
