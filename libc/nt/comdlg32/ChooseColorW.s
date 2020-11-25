.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ChooseColorW,ChooseColorW,103

	.text.windows
ChooseColor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseColorW(%rip)
	leave
	ret
	.endfn	ChooseColor,globl
	.previous
