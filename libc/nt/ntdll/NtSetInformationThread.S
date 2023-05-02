#include "libc/nt/ntdllimport.h"
.ntimp	NtSetInformationThread,NtSetInformationThread

#ifdef __x86_64__
	.text.windows
NtSetInformationThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtSetInformationThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtSetInformationThread,globl
	.previous
#endif
