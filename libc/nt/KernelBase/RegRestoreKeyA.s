.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegRestoreKeyA,RegRestoreKeyA,1373

	.text.windows
RegRestoreKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegRestoreKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegRestoreKeyA,globl
	.previous
