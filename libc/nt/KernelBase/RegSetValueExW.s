.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegSetValueExW,RegSetValueExW,1381

	.text.windows
RegSetValueEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegSetValueExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegSetValueEx,globl
	.previous
