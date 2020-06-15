.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DispatchMessageA,DispatchMessageA,1697

	.text.windows
DispatchMessageA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DispatchMessageA(%rip)
	leave
	ret
	.endfn	DispatchMessageA,globl
	.previous
