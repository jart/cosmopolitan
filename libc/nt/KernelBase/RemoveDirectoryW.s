.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RemoveDirectoryW,RemoveDirectoryW,1400

	.text.windows
RemoveDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RemoveDirectoryW(%rip)
	leave
	ret
	.endfn	RemoveDirectory,globl
	.previous
