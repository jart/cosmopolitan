#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationProcess,NtQueryInformationProcess

#ifdef __x86_64__
	.text.windows
NtQueryInformationProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationProcess(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationProcess,globl
	.previous
#endif
