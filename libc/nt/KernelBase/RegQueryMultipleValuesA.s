.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryMultipleValuesA,RegQueryMultipleValuesA,1369

	.text.windows
RegQueryMultipleValuesA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryMultipleValuesA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryMultipleValuesA,globl
	.previous
