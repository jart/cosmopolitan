.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegEnumKeyExA,RegEnumKeyExA,1338

	.text.windows
RegEnumKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumKeyExA,globl
	.previous
