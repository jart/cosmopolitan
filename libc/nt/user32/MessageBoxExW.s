.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_MessageBoxExW,MessageBoxExW,2165

	.text.windows
MessageBoxEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MessageBoxExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MessageBoxEx,globl
	.previous
