.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_BringWindowToTop,BringWindowToTop,1523

	.text.windows
BringWindowToTop:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_BringWindowToTop(%rip)
	leave
	ret
	.endfn	BringWindowToTop,globl
	.previous
