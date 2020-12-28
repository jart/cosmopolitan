.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_RemoveDirectoryA,RemoveDirectoryA,0

	.text.windows
RemoveDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_RemoveDirectoryA(%rip)
	leave
	ret
	.endfn	RemoveDirectoryA,globl
	.previous
