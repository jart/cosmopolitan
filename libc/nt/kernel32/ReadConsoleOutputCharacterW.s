.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleOutputCharacterW,ReadConsoleOutputCharacterW,0

	.text.windows
ReadConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputCharacter,globl
	.previous
