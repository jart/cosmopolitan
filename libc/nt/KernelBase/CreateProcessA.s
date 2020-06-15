.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateProcessA,CreateProcessA,207

	.text.windows
CreateProcessA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateProcessA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	CreateProcessA,globl
	.previous
