.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadConsoleOutputCharacterA,ReadConsoleOutputCharacterA,0

	.text.windows
ReadConsoleOutputCharacterA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputCharacterA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputCharacterA,globl
	.previous
