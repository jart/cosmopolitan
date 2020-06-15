.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_SetBkMode,SetBkMode,1872

	.text.windows
SetBkMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetBkMode(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetBkMode,globl
	.previous
