.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegOpenUserClassesRoot,RegOpenUserClassesRoot,0

	.text.windows
RegOpenUserClassesRoot:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenUserClassesRoot(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegOpenUserClassesRoot,globl
	.previous
