.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ChooseFontA,ChooseFontA,104

	.text.windows
ChooseFontA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ChooseFontA(%rip)
	leave
	ret
	.endfn	ChooseFontA,globl
	.previous
