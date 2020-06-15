.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_BeginPaint,BeginPaint,1521

	.text.windows
BeginPaint:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_BeginPaint(%rip),%rax
	jmp	__sysv2nt
	.endfn	BeginPaint,globl
	.previous
