#include "libc/nt/ntdllimport.h"
.ntimp	RtlCreateProcessParameters,RtlCreateProcessParameters

#ifdef __x86_64__
	.text.windows
RtlCreateProcessParameters:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCreateProcessParameters(%rip),%rax
	jmp	__sysv2nt10
	.endfn	RtlCreateProcessParameters,globl
	.previous
#endif
