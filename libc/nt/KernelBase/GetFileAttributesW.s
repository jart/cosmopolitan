.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileAttributesW,GetFileAttributesW,546

	.text.windows
GetFileAttributes:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetFileAttributesW(%rip)
	leave
	ret
	.endfn	GetFileAttributes,globl
	.previous
