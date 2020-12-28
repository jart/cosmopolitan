.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegLoadKeyW,RegLoadKeyW,0

	.text.windows
RegLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegLoadKey,globl
	.previous
