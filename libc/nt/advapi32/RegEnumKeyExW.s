.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumKeyExW,RegEnumKeyExW,0

	.text.windows
RegEnumKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumKeyEx,globl
	.previous
