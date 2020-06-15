.include "o/libc/nt/ntdllimport.inc"
.ntimp	RtlInitializeCriticalSection

	.text.windows
RtlInitializeCriticalSection:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RtlInitializeCriticalSection(%rip)
	leave
	ret
	.endfn	RtlInitializeCriticalSection,globl
	.previous
