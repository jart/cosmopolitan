.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleCtrlHandler,SetConsoleCtrlHandler,0

	.text.windows
SetConsoleCtrlHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCtrlHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCtrlHandler,globl
	.previous
