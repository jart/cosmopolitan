.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DeleteFileA,DeleteFileA,0

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
