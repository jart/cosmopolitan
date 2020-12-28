.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryMultipleValuesA,RegQueryMultipleValuesA,0

	.text.windows
RegQueryMultipleValuesA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryMultipleValuesA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryMultipleValuesA,globl
	.previous
