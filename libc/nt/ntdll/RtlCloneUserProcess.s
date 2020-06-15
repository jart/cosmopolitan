.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlCloneUserProcess

	.text.windows
RtlCloneUserProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCloneUserProcess(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RtlCloneUserProcess,globl
	.previous
