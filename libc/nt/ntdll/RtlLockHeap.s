.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlLockHeap

	.text.windows
RtlLockHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlLockHeap(%rip)
	leave
	ret
	.endfn	RtlLockHeap,globl
	.previous
