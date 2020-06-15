.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlUnlockHeap

	.text.windows
RtlUnlockHeap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlUnlockHeap(%rip)
	leave
	ret
	.endfn	RtlUnlockHeap,globl
	.previous
