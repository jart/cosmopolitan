.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegRestoreKeyW,RegRestoreKeyW,0

	.text.windows
RegRestoreKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegRestoreKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegRestoreKey,globl
	.previous
