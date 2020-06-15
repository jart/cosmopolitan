.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowsHookW,SetWindowsHookW,2410

	.text.windows
SetWindowsHook:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHook,globl
	.previous
