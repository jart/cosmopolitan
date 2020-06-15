.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlValidateHeap

	.text.windows
RtlValidateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlValidateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlValidateHeap,globl
	.previous
