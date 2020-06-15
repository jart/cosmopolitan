.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegSetKeySecurity,RegSetKeySecurity,1377

	.text.windows
RegSetKeySecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetKeySecurity(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegSetKeySecurity,globl
	.previous
