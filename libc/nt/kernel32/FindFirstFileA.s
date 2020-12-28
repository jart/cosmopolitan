.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FindFirstFileA,FindFirstFileA,0

	.text.windows
FindFirstFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileA(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindFirstFileA,globl
	.previous
