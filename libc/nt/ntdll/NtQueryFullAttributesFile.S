#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryFullAttributesFile,NtQueryFullAttributesFile

	.text.windows
NtQueryFullAttributesFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryFullAttributesFile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryFullAttributesFile,globl
	.previous
