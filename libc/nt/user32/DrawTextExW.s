.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_DrawTextExW,DrawTextExW,1730

	.text.windows
DrawTextEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawTextEx,globl
	.previous
