.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetFileAttributesA,GetFileAttributesA,0

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
