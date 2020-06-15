.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_RegDeleteKeyExW,RegDeleteKeyExW,1330

	.text.windows
RegDeleteKeyEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegDeleteKeyExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	RegDeleteKeyEx,globl
	.previous
