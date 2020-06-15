.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryMultipleValuesW,RegQueryMultipleValuesW,1370

	.text.windows
RegQueryMultipleValues:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryMultipleValuesW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryMultipleValues,globl
	.previous
