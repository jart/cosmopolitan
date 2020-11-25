.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_PrintDlgA,PrintDlgA,120

	.text.windows
PrintDlgA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PrintDlgA(%rip)
	leave
	ret
	.endfn	PrintDlgA,globl
	.previous
