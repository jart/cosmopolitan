.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SwapBuffers,SwapBuffers,1929

	.text.windows
SwapBuffers:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SwapBuffers(%rip)
	leave
	ret
	.endfn	SwapBuffers,globl
	.previous
