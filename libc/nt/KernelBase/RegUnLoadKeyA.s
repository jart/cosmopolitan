.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegUnLoadKeyA,RegUnLoadKeyA,1382

	.text.windows
RegUnLoadKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegUnLoadKeyA,globl
	.previous
