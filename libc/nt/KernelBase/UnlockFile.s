.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_UnlockFile,UnlockFile,1693

	.text.windows
UnlockFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	UnlockFile,globl
	.previous
