.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryValueExW,RegQueryValueExW,1372

	.text.windows
RegQueryValueEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryValueExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegQueryValueEx,globl
	.previous
