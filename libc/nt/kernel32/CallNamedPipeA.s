.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CallNamedPipeA,CallNamedPipeA,110

	.text.windows
CallNamedPipeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNamedPipeA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CallNamedPipeA,globl
	.previous
