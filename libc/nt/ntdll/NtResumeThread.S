#include "libc/nt/ntdllimport.h"
.ntimp	NtResumeThread,NtResumeThread

#ifdef __x86_64__
	.text.windows
NtResumeThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_NtResumeThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	NtResumeThread,globl
	.previous
#endif
