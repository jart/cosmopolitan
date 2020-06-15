.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtYieldExecution

	.text.windows
NtYieldExecution:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_NtYieldExecution(%rip)
	leave
	ret
	.endfn	NtYieldExecution,globl
	.previous
