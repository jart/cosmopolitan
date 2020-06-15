.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_PeekConsoleInputW,PeekConsoleInputW,1204

	.text.windows
PeekConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PeekConsoleInput,globl
	.previous
