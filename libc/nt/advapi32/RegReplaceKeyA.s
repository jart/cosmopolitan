.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegReplaceKeyA,RegReplaceKeyA,1669

	.text.windows
RegReplaceKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegReplaceKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegReplaceKeyA,globl
	.previous
