.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegLoadKeyW,RegLoadKeyW,1357

	.text.windows
RegLoadKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegLoadKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegLoadKey,globl
	.previous
