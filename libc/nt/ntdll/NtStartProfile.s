.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtStartProfile

	.text.windows
NtStartProfile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtStartProfile(%rip)
	leave
	ret
	.endfn	NtStartProfile,globl
	.previous
