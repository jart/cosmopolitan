.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegSaveKeyA,RegSaveKeyA,1673

	.text.windows
RegSaveKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSaveKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegSaveKeyA,globl
	.previous
