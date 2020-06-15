.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowsHookA,SetWindowsHookA,2407

	.text.windows
SetWindowsHookA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowsHookA(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowsHookA,globl
	.previous
