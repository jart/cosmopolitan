.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowsHookExW,SetWindowsHookExW,2409

	.text.windows
SetWindowsHookEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHookEx,globl
	.previous
