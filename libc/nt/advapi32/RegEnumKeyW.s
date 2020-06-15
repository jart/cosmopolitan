.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumKeyW,RegEnumKeyW,1636

	.text.windows
RegEnumKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegEnumKey,globl
	.previous
