.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegOpenKeyExW,RegOpenKeyExW,1365

	.text.windows
RegOpenKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenKeyExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegOpenKeyEx,globl
	.previous
