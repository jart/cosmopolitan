.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_PeekConsoleInputW,PeekConsoleInputW,0

	.text.windows
PeekConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PeekConsoleInput,globl
	.previous
