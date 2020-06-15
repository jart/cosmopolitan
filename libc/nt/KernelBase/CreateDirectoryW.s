.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateDirectoryW,CreateDirectoryW,179

	.text.windows
CreateDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateDirectory,globl
	.previous
