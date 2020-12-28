.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegCreateKeyExW,RegCreateKeyExW,0

	.text.windows
RegCreateKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyExW(%rip),%rax
	jmp	__sysv2nt10
	.endfn	RegCreateKeyEx,globl
	.previous
