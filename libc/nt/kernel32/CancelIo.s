.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CancelIo,CancelIo,0

	.text.windows
CancelIo:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_CancelIo(%rip)
	leave
	ret
	.endfn	CancelIo,globl
	.previous
