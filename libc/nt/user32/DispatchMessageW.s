.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DispatchMessageW,DispatchMessageW,1698

	.text.windows
DispatchMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DispatchMessageW(%rip)
	leave
	ret
	.endfn	DispatchMessage,globl
	.previous
