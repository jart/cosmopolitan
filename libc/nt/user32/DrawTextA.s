.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DrawTextA,DrawTextA,1728

	.text.windows
DrawTextA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawTextA,globl
	.previous
