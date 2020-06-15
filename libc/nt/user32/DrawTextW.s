.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DrawTextW,DrawTextW,1731

	.text.windows
DrawText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawText,globl
	.previous
