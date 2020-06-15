.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtQuerySystemTime

	.text.windows
NtQuerySystemTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_NtQuerySystemTime(%rip)
	leave
	ret
	.endfn	NtQuerySystemTime,globl
	.previous
