.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SetTextJustification,SetTextJustification,1913

	.text.windows
SetTextJustification:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetTextJustification(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetTextJustification,globl
	.previous
