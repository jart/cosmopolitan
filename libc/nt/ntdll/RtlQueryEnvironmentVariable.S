#include "libc/nt/ntdllimport.h"
.ntimp	RtlQueryEnvironmentVariable,RtlQueryEnvironmentVariable

#ifdef __x86_64__
	.text.windows
RtlQueryEnvironmentVariable:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlQueryEnvironmentVariable(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlQueryEnvironmentVariable,globl
	.previous
#endif
