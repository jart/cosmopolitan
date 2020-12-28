.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteValueA,RegDeleteValueA,0

	.text.windows
RegDeleteValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteValueA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteValueA,globl
	.previous
