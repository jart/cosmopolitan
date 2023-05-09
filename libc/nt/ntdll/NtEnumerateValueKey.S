#include "libc/nt/ntdllimport.h"
.ntimp	NtEnumerateValueKey,NtEnumerateValueKey

	.text.windows
NtEnumerateValueKey:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtEnumerateValueKey(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	NtEnumerateValueKey,globl
	.previous
