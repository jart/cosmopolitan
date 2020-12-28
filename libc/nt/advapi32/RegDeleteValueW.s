.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteValueW,RegDeleteValueW,0

	.text.windows
RegDeleteValue:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteValueW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteValue,globl
	.previous
