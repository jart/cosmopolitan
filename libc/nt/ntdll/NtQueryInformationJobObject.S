#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationJobObject,NtQueryInformationJobObject

#ifdef __x86_64__
	.text.windows
NtQueryInformationJobObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationJobObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationJobObject,globl
	.previous
#endif
