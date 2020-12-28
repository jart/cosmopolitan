.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegQueryInfoKeyA,RegQueryInfoKeyA,0

	.text.windows
RegQueryInfoKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryInfoKeyA(%rip),%rax
	jmp	__sysv2nt12
	.endfn	RegQueryInfoKeyA,globl
	.previous
