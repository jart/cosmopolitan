.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_MoveFileW,MoveFileW,1000

	.text.windows
MoveFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	MoveFile,globl
	.previous
