.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_AppendMenuW,AppendMenuW,1516

	.text.windows
AppendMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuW(%rip),%rax
	jmp	__sysv2nt
	.endfn	AppendMenu,globl
	.previous
