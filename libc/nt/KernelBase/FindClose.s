.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindClose,FindClose,359

	.text.windows
FindClose:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FindClose(%rip)
	leave
	ret
	.endfn	FindClose,globl
	.previous
