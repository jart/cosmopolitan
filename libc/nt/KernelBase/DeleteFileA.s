.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DeleteFileA,DeleteFileA,253

	.text.windows
DeleteFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteFileA(%rip)
	leave
	ret
	.endfn	DeleteFileA,globl
	.previous
