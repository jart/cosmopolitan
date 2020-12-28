.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteConsoleOutputCharacterW,WriteConsoleOutputCharacterW,0

	.text.windows
WriteConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputCharacter,globl
	.previous
