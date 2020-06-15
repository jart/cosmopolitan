.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateFileA,CreateFileA,188

	.text.windows
CreateFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateFileA,globl
	.previous
