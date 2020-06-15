.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_SetWindowTextW,SetWindowTextW,2405

	.text.windows
SetWindowText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowTextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetWindowText,globl
	.previous
