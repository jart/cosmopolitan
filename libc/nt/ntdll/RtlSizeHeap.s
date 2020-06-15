.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlSizeHeap

	.text.windows
RtlSizeHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlSizeHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlSizeHeap,globl
	.previous
