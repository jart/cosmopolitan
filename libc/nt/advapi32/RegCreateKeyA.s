.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegCreateKeyA,RegCreateKeyA,1611

	.text.windows
RegCreateKeyA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyA(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegCreateKeyA,globl
	.previous
