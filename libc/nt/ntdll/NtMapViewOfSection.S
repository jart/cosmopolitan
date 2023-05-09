#include "libc/nt/ntdllimport.h"
.ntimp	NtMapViewOfSection,NtMapViewOfSection

	.text.windows
NtMapViewOfSection:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtMapViewOfSection(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtMapViewOfSection,globl
	.previous
