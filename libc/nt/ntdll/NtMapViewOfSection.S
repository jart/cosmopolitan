#include "libc/nt/ntdllimport.h"
.ntimp	NtMapViewOfSection,NtMapViewOfSection

#ifdef __x86_64__
	.text.windows
NtMapViewOfSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtMapViewOfSection(%rip),%rax
	jmp	__sysv2nt10
	.endfn	NtMapViewOfSection,globl
	.previous
#endif
