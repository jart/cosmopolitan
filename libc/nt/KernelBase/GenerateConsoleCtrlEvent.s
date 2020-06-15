.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GenerateConsoleCtrlEvent,GenerateConsoleCtrlEvent,410

	.text.windows
GenerateConsoleCtrlEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GenerateConsoleCtrlEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	GenerateConsoleCtrlEvent,globl
	.previous
