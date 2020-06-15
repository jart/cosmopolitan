.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowsHookExA,SetWindowsHookExA,2408

	.text.windows
SetWindowsHookExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHookExA,globl
	.previous
