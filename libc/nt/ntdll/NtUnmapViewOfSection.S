#include "libc/nt/ntdllimport.h"
.ntimp	NtUnmapViewOfSection,NtUnmapViewOfSection

	.text.windows
	.ftrace1
NtUnmapViewOfSection:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_NtUnmapViewOfSection(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtUnmapViewOfSection,globl
	.previous
