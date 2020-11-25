.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetOpenFileNameW,GetOpenFileNameW,114

	.text.windows
GetOpenFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetOpenFileNameW(%rip)
	leave
	ret
	.endfn	GetOpenFileName,globl
	.previous
