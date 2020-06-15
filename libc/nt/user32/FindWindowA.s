.include "o/libc/nt/codegen.inc"
.imp	user32,__imp_FindWindowA,FindWindowA,1781

	.text.windows
FindWindowA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindowA,globl
	.previous
