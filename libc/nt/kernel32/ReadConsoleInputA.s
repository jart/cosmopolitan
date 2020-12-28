.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleInputA,ReadConsoleInputA,0

	.text.windows
ReadConsoleInputA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleInputA(%rip),%rax
	jmp	__sysv2nt
	.endfn	ReadConsoleInputA,globl
	.previous
