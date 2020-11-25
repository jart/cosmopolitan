.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ReplaceTextW,ReplaceTextW,125

	.text.windows
ReplaceText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ReplaceTextW(%rip)
	leave
	ret
	.endfn	ReplaceText,globl
	.previous
