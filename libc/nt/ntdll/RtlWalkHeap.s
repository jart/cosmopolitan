.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlWalkHeap

	.text.windows
RtlWalkHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlWalkHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlWalkHeap,globl
	.previous
