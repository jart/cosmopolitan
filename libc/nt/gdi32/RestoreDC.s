.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_RestoreDC,RestoreDC,1808

	.text.windows
RestoreDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RestoreDC(%rip),%rax
	jmp	__sysv2nt
	.endfn	RestoreDC,globl
	.previous
