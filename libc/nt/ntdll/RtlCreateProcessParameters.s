.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlCreateProcessParameters

	.text.windows
RtlCreateProcessParameters:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCreateProcessParameters(%rip),%rax
	jmp	__sysv2nt10
	.endfn	RtlCreateProcessParameters,globl
	.previous
