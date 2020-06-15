.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryInfoKeyA,RegQueryInfoKeyA,1367

	.text.windows
RegQueryInfoKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryInfoKeyA(%rip),%rax
	jmp	__sysv2nt12
	.endfn	RegQueryInfoKeyA,globl
	.previous
