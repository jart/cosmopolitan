.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreatePipe,CreatePipe,202

	.text.windows
CreatePipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreatePipe(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreatePipe,globl
	.previous
