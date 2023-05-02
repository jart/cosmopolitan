#include "libc/nt/ntdllimport.h"
.ntimp	NtQueryInformationThread,NtQueryInformationThread

#ifdef __x86_64__
	.text.windows
NtQueryInformationThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtQueryInformationThread(%rip),%rax
	jmp	__sysv2nt6
	.endfn	NtQueryInformationThread,globl
	.previous
#endif
