.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CallNamedPipeW,CallNamedPipeW,99

	.text.windows
CallNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CallNamedPipe,globl
	.previous
