.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegDeleteValueA,RegDeleteValueA,1335

	.text.windows
RegDeleteValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteValueA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteValueA,globl
	.previous
