.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlReAllocateHeap

	.text.windows
RtlReAllocateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlReAllocateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlReAllocateHeap,globl
	.previous
