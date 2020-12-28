.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FillConsoleOutputCharacterA,FillConsoleOutputCharacterA,0

	.text.windows
FillConsoleOutputCharacterA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputCharacterA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputCharacterA,globl
	.previous
