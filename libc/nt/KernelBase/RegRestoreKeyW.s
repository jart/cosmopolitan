.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegRestoreKeyW,RegRestoreKeyW,1374

	.text.windows
RegRestoreKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegRestoreKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegRestoreKey,globl
	.previous
