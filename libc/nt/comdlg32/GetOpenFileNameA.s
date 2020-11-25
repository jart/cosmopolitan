.include "o/libc/nt/codegen.inc"
.imp	comdlg32,__imp_GetOpenFileNameA,GetOpenFileNameA,113

	.text.windows
GetOpenFileNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetOpenFileNameA(%rip)
	leave
	ret
	.endfn	GetOpenFileNameA,globl
	.previous
