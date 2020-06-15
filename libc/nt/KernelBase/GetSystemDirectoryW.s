.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetSystemDirectoryW,GetSystemDirectoryW,738

	.text.windows
GetSystemDirectory:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemDirectory,globl
	.previous
