.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_PrintDlgW,PrintDlgW,123

	.text.windows
PrintDlg:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_PrintDlgW(%rip)
	leave
	ret
	.endfn	PrintDlg,globl
	.previous
