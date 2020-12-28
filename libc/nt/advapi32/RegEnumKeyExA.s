.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumKeyExA,RegEnumKeyExA,0

	.text.windows
RegEnumKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumKeyExA,globl
	.previous
