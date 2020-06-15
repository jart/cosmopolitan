.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryValueExA,RegQueryValueExA,1371

	.text.windows
RegQueryValueExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryValueExA,globl
	.previous
