.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_BitBlt,BitBlt,1034

	.text.windows
BitBlt:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_BitBlt(%rip),%rax
	jmp	__sysv2nt10
	.endfn	BitBlt,globl
	.previous
