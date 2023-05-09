#include "libc/nt/ntdllimport.h"
.ntimp	RtlCreateProcessParameters,RtlCreateProcessParameters

	.text.windows
RtlCreateProcessParameters:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCreateProcessParameters(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	RtlCreateProcessParameters,globl
	.previous
