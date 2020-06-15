.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegGetValueA,RegGetValueA,1344

	.text.windows
RegGetValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetValueA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegGetValueA,globl
	.previous
