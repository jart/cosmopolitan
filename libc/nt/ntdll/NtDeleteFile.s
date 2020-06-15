.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtDeleteFile

	.text.windows
NtDeleteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtDeleteFile(%rip)
	leave
	ret
	.endfn	NtDeleteFile,globl
	.previous
