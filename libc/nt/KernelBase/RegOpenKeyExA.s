.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegOpenKeyExA,RegOpenKeyExA,1362

	.text.windows
RegOpenKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenKeyExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegOpenKeyExA,globl
	.previous
