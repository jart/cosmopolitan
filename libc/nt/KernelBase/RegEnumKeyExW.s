.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegEnumKeyExW,RegEnumKeyExW,1339

	.text.windows
RegEnumKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegEnumKeyExW(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegEnumKeyEx,globl
	.previous
