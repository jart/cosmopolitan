.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_ReplaceTextA,ReplaceTextA,124

	.text.windows
ReplaceTextA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ReplaceTextA(%rip)
	leave
	ret
	.endfn	ReplaceTextA,globl
	.previous
