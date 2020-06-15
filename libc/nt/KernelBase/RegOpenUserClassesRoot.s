.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegOpenUserClassesRoot,RegOpenUserClassesRoot,1366

	.text.windows
RegOpenUserClassesRoot:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenUserClassesRoot(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOpenUserClassesRoot,globl
	.previous
