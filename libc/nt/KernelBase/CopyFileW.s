.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CopyFileW,CopyFileW,168

	.text.windows
CopyFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CopyFileW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CopyFile,globl
	.previous
