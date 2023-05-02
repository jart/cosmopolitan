#include "libc/nt/ntdllimport.h"
.ntimp	NtUnmapViewOfSection,NtUnmapViewOfSection

#ifdef __x86_64__
	.text.windows
NtUnmapViewOfSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtUnmapViewOfSection(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtUnmapViewOfSection,globl
	.previous
#endif
