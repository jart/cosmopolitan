.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_EndPaint,EndPaint,1752

	.text.windows
EndPaint:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_EndPaint(%rip),%rax
	jmp	__sysv2nt
	.endfn	EndPaint,globl
	.previous
