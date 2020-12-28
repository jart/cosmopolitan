.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleA,WriteConsoleA,0

	.text.windows
WriteConsoleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleA,globl
	.previous
