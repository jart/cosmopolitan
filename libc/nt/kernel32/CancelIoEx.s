.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CancelIoEx,CancelIoEx,0

	.text.windows
CancelIoEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CancelIoEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	CancelIoEx,globl
	.previous
