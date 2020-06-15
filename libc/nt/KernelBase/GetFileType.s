.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileType,GetFileType,555

	.text.windows
GetFileType:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetFileType(%rip)
	leave
	ret
	.endfn	GetFileType,globl
	.previous
