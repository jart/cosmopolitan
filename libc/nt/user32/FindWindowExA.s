.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_FindWindowExA,FindWindowExA,1782

	.text.windows
FindWindowExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindowExA,globl
	.previous
