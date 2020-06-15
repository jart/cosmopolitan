.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegSetValueExA,RegSetValueExA,1380

	.text.windows
RegSetValueExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValueExA,globl
	.previous
