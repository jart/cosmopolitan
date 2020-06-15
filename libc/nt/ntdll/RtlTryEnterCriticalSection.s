.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlTryEnterCriticalSection

	.text.windows
RtlTryEnterCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlTryEnterCriticalSection(%rip)
	leave
	ret
	.endfn	RtlTryEnterCriticalSection,globl
	.previous
