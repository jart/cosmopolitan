.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegCreateKeyW,RegCreateKeyW,1616

	.text.windows
RegCreateKey:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegCreateKeyW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegCreateKey,globl
	.previous
