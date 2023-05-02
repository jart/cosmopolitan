#include "libc/nt/ntdllimport.h"
.ntimp	NtEnumerateKey,NtEnumerateKey

#ifdef __x86_64__
	.text.windows
NtEnumerateKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtEnumerateKey(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtEnumerateKey,globl
	.previous
#endif
