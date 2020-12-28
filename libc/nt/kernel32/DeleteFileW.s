.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_DeleteFileW,DeleteFileW,0

	.text.windows
DeleteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteFileW(%rip)
	leave
	ret
	.endfn	DeleteFile,globl
	.previous
