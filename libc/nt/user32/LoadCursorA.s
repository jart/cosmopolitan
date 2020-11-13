.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_LoadCursorA,LoadCursorA,2098

	.text.windows
LoadCursorA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadCursorA(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadCursorA,globl
	.previous
