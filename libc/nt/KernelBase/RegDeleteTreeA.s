.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegDeleteTreeA,RegDeleteTreeA,1333

	.text.windows
RegDeleteTreeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteTreeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteTreeA,globl
	.previous
