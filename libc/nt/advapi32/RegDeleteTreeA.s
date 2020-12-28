.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteTreeA,RegDeleteTreeA,0

	.text.windows
RegDeleteTreeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteTreeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteTreeA,globl
	.previous
