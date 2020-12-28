.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetNumberOfConsoleInputEvents,GetNumberOfConsoleInputEvents,0

	.text.windows
GetNumberOfConsoleInputEvents:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetNumberOfConsoleInputEvents(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetNumberOfConsoleInputEvents,globl
	.previous
