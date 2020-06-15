.include "o/libc/nt/ntdllimport.inc"
.ntimp	NtTestAlert

	.text.windows
NtTestAlert:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_NtTestAlert(%rip)
	leave
	ret
	.endfn	NtTestAlert,globl
	.previous
