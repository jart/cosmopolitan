.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetCurrentDirectoryW,SetCurrentDirectoryW,0

	.text.windows
SetCurrentDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetCurrentDirectoryW(%rip)
	leave
	ret
	.endfn	SetCurrentDirectory,globl
	.previous
