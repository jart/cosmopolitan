.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LockFile,LockFile,0

	.text.windows
LockFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LockFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	LockFile,globl
	.previous
