.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlAllocateHeap

	.text.windows
RtlAllocateHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlAllocateHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlAllocateHeap,globl
	.previous
