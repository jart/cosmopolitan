.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleOutputCharacterA,WriteConsoleOutputCharacterA,0

	.text.windows
WriteConsoleOutputCharacterA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputCharacterA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputCharacterA,globl
	.previous
