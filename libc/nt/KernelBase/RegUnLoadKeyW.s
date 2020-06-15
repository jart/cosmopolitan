.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegUnLoadKeyW,RegUnLoadKeyW,1383

	.text.windows
RegUnLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegUnLoadKey,globl
	.previous
