.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetSaveFileNameW,GetSaveFileNameW,116

	.text.windows
GetSaveFileName:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSaveFileNameW(%rip)
	leave
	ret
	.endfn	GetSaveFileName,globl
	.previous
