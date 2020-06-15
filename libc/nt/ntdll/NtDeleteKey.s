.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtDeleteKey

	.text.windows
NtDeleteKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtDeleteKey(%rip)
	leave
	ret
	.endfn	NtDeleteKey,globl
	.previous
