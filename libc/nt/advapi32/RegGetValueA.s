.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegGetValueA,RegGetValueA,0

	.text.windows
RegGetValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegGetValueA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	RegGetValueA,globl
	.previous
