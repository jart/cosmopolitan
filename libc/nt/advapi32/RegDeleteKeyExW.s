.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_RegDeleteKeyExW,RegDeleteKeyExW,0

	.text.windows
RegDeleteKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKeyEx,globl
	.previous
