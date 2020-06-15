.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetCurrentDirectoryW,GetCurrentDirectoryW,489

	.text.windows
GetCurrentDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCurrentDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCurrentDirectory,globl
	.previous
