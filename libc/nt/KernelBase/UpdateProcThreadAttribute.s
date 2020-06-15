.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_UpdateProcThreadAttribute,UpdateProcThreadAttribute,1709

	.text.windows
UpdateProcThreadAttribute:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UpdateProcThreadAttribute(%rip),%rax
	jmp	__sysv2nt8
	.endfn	UpdateProcThreadAttribute,globl
	.previous
