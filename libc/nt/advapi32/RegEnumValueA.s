.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumValueA,RegEnumValueA,0

	.text.windows
RegEnumValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumValueA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumValueA,globl
	.previous
