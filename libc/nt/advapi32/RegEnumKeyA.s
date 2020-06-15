.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegEnumKeyA,RegEnumKeyA,1633

	.text.windows
RegEnumKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegEnumKeyA,globl
	.previous
