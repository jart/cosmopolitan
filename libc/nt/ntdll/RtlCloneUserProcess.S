#include "libc/nt/ntdllimport.h"
.ntimp	RtlCloneUserProcess,RtlCloneUserProcess

#ifdef __x86_64__
	.text.windows
RtlCloneUserProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCloneUserProcess(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RtlCloneUserProcess,globl
	.previous
#endif
