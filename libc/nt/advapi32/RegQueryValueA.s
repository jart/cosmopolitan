.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryValueA,RegQueryValueA,1664

	.text.windows
RegQueryValueA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegQueryValueA,globl
	.previous
