#include "libc/nt/ntdllimport.h"
.ntimp	NtUnmapViewOfSection,NtUnmapViewOfSection

	.text.windows
NtUnmapViewOfSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtUnmapViewOfSection(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtUnmapViewOfSection,globl
	.previous
