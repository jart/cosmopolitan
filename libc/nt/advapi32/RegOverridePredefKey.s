.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegOverridePredefKey,RegOverridePredefKey,1658

	.text.windows
RegOverridePredefKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOverridePredefKey(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOverridePredefKey,globl
	.previous
