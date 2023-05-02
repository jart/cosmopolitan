#include "libc/nt/ntdllimport.h"
.ntimp	NtEnumerateValueKey,NtEnumerateValueKey

#ifdef __x86_64__
	.text.windows
NtEnumerateValueKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtEnumerateValueKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtEnumerateValueKey,globl
	.previous
#endif
