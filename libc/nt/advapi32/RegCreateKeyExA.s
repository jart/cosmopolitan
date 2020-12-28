.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegCreateKeyExA,RegCreateKeyExA,0

	.text.windows
RegCreateKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyExA(%rip),%rax
	jmp	__sysv2nt10
	.endfn	RegCreateKeyExA,globl
	.previous
