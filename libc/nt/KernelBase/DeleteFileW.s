.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_DeleteFileW,DeleteFileW,254

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
