.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_CancelIoEx,CancelIoEx,102

	.text.windows
CancelIoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CancelIoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	CancelIoEx,globl
	.previous
