.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_CallNextHookEx,CallNextHookEx,1535

	.text.windows
CallNextHookEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CallNextHookEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	CallNextHookEx,globl
	.previous
