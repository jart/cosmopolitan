.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteTreeW,RegDeleteTreeW,0

	.text.windows
RegDeleteTree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteTreeW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteTree,globl
	.previous
