.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreateFileA,CreateFileA,0

	.text.windows
CreateFileA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateFileA(%rip),%rax
	jmp	__sysv2nt8
	.endfn	CreateFileA,globl
	.previous
