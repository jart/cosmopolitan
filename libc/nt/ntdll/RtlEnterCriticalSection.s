.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlEnterCriticalSection

	.text.windows
RtlEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlEnterCriticalSection(%rip)
	leave
	ret
	.endfn	RtlEnterCriticalSection,globl
	.previous
