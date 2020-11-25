.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ChooseFontW,ChooseFontW,105

	.text.windows
ChooseFont:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseFontW(%rip)
	leave
	ret
	.endfn	ChooseFont,globl
	.previous
