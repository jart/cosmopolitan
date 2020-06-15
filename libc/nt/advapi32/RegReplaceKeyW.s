.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegReplaceKeyW,RegReplaceKeyW,1670

	.text.windows
RegReplaceKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegReplaceKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegReplaceKey,globl
	.previous
