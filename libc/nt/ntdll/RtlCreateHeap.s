.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlCreateHeap

	.text.windows
RtlCreateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlCreateHeap(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RtlCreateHeap,globl
	.previous
