.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegOpenKeyExA,RegOpenKeyExA,0

	.text.windows
RegOpenKeyExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegOpenKeyExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegOpenKeyExA,globl
	.previous
