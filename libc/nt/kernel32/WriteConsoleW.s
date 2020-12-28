.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleW,WriteConsoleW,0

	.text.windows
WriteConsole:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsole,globl
	.previous
