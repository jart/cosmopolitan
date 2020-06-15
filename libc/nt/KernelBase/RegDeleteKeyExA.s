.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegDeleteKeyExA,RegDeleteKeyExA,1327

	.text.windows
RegDeleteKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKeyExA,globl
	.previous
