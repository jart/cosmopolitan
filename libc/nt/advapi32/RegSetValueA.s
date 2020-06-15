.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegSetValueA,RegSetValueA,1680

	.text.windows
RegSetValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValueA,globl
	.previous
