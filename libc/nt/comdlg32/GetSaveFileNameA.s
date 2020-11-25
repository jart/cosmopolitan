.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetSaveFileNameA,GetSaveFileNameA,115

	.text.windows
GetSaveFileNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetSaveFileNameA(%rip)
	leave
	ret
	.endfn	GetSaveFileNameA,globl
	.previous
