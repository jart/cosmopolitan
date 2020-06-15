.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlFreeHeap

	.text.windows
RtlFreeHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RtlFreeHeap(%rip),%rax
	jmp	__sysv2nt
	.endfn	RtlFreeHeap,globl
	.previous
