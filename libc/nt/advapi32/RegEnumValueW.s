.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumValueW,RegEnumValueW,0

	.text.windows
RegEnumValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumValueW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumValue,globl
	.previous
