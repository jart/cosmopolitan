.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CreateIoCompletionPort,CreateIoCompletionPort,195

	.text.windows
CreateIoCompletionPort:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateIoCompletionPort(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateIoCompletionPort,globl
	.previous
