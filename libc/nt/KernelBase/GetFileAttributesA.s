.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileAttributesA,GetFileAttributesA,543

	.text.windows
GetFileAttributesA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetFileAttributesA(%rip)
	leave
	ret
	.endfn	GetFileAttributesA,globl
	.previous
