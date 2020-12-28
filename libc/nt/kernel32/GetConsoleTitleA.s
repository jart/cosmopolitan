.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetConsoleTitleA,GetConsoleTitleA,0

	.text.windows
GetConsoleTitleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleTitleA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetConsoleTitleA,globl
	.previous
