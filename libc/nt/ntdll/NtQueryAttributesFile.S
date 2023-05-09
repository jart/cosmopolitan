#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryAttributesFile,NtQueryAttributesFile

	.text.windows
NtQueryAttributesFile:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryAttributesFile(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtQueryAttributesFile,globl
	.previous
