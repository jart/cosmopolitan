.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegRestoreKeyA,RegRestoreKeyA,0

	.text.windows
RegRestoreKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegRestoreKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegRestoreKeyA,globl
	.previous
