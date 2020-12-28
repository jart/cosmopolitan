.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegUnLoadKeyW,RegUnLoadKeyW,0

	.text.windows
RegUnLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegUnLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegUnLoadKey,globl
	.previous
