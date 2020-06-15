.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegQueryInfoKeyW,RegQueryInfoKeyW,1368

	.text.windows
RegQueryInfoKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegQueryInfoKeyW(%rip),%rax
	jmp	__sysv2nt12
	.endfn	RegQueryInfoKey,globl
	.previous
