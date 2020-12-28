.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_UnlockFile,UnlockFile,0

	.text.windows
UnlockFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	UnlockFile,globl
	.previous
