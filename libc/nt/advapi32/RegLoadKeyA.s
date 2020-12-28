.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegLoadKeyA,RegLoadKeyA,0

	.text.windows
RegLoadKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegLoadKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegLoadKeyA,globl
	.previous
