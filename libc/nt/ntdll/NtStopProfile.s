.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtStopProfile

	.text.windows
NtStopProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtStopProfile(%rip)
	leave
	ret
	.endfn	NtStopProfile,globl
	.previous
