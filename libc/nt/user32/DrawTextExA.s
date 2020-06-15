.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DrawTextExA,DrawTextExA,1729

	.text.windows
DrawTextExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextExA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawTextExA,globl
	.previous
