.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleA,ReadConsoleA,0

	.text.windows
ReadConsoleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleA,globl
	.previous
