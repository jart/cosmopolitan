.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadConsoleOutputCharacterW,ReadConsoleOutputCharacterW,1306

	.text.windows
ReadConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputCharacter,globl
	.previous
