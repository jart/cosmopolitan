.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_LoadResource,LoadResource,980

	.text.windows
LoadResource:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadResource(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadResource,globl
	.previous
