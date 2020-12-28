.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegSetValueExA,RegSetValueExA,0

	.text.windows
RegSetValueExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValueExA,globl
	.previous
