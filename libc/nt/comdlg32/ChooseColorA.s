.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ChooseColorA,ChooseColorA,102

	.text.windows
ChooseColorA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseColorA(%rip)
	leave
	ret
	.endfn	ChooseColorA,globl
	.previous
