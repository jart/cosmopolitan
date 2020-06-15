.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegEnumValueW,RegEnumValueW,1341

	.text.windows
RegEnumValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumValueW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumValue,globl
	.previous
