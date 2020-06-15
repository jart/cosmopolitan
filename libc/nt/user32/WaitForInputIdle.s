.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_WaitForInputIdle,WaitForInputIdle,2495

	.text.windows
WaitForInputIdle:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForInputIdle(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForInputIdle,globl
	.previous
