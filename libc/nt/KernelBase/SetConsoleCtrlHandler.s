.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_SetConsoleCtrlHandler,SetConsoleCtrlHandler,1474

	.text.windows
SetConsoleCtrlHandler:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetConsoleCtrlHandler(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetConsoleCtrlHandler,globl
	.previous
