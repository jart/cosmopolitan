.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlDeleteCriticalSection

	.text.windows
RtlDeleteCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlDeleteCriticalSection(%rip)
	leave
	ret
	.endfn	RtlDeleteCriticalSection,globl
	.previous
