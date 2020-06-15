.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlLeaveCriticalSection

	.text.windows
RtlLeaveCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlLeaveCriticalSection(%rip)
	leave
	ret
	.endfn	RtlLeaveCriticalSection,globl
	.previous
