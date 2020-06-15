.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegOpenKeyA,RegOpenKeyA,1651

	.text.windows
RegOpenKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOpenKeyA,globl
	.previous
