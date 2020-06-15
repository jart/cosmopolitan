.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateNamedPipeW,CreateNamedPipeW,201

	.text.windows
CreateNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateNamedPipe,globl
	.previous
