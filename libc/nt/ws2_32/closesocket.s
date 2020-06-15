.include "o/libc/nt/codegen.inc"
.imp	ws2_32,__imp_closesocket,closesocket,3

	.text.windows
__closesocket$nt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_closesocket(%rip)
	leave
	ret
	.endfn	__closesocket$nt,globl
	.previous
