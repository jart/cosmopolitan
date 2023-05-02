#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationToken,NtQueryInformationToken

#ifdef __x86_64__
	.text.windows
NtQueryInformationToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationToken(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationToken,globl
	.previous
#endif
