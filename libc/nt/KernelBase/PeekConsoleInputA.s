.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PeekConsoleInputA,PeekConsoleInputA,1203

	.text.windows
PeekConsoleInputA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekConsoleInputA(%rip),%rax
	jmp	__sysv2nt
	.endfn	PeekConsoleInputA,globl
	.previous
