.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GenerateConsoleCtrlEvent,GenerateConsoleCtrlEvent,0

	.text.windows
GenerateConsoleCtrlEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GenerateConsoleCtrlEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	GenerateConsoleCtrlEvent,globl
	.previous
