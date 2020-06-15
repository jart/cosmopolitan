.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_TransactNamedPipe,TransactNamedPipe,1684

	.text.windows
TransactNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TransactNamedPipe(%rip),%rax
	jmp	__sysv2nt8
	.endfn	TransactNamedPipe,globl
	.previous
