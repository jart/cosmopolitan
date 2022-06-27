.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_UnmapViewOfFile,UnmapViewOfFile,0

	.text.windows
__UnmapViewOfFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_UnmapViewOfFile(%rip)
	leave
	ret
	.endfn	__UnmapViewOfFile,globl
	.previous
