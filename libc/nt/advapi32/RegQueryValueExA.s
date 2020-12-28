.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryValueExA,RegQueryValueExA,0

	.text.windows
RegQueryValueExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryValueExA,globl
	.previous
