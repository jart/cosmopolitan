.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegUnLoadKeyA,RegUnLoadKeyA,0

	.text.windows
RegUnLoadKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegUnLoadKeyA,globl
	.previous
