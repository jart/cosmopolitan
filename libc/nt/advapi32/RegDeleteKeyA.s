.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteKeyA,RegDeleteKeyA,1617

	.text.windows
RegDeleteKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKeyA,globl
	.previous
