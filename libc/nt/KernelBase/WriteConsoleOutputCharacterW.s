.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_WriteConsoleOutputCharacterW,WriteConsoleOutputCharacterW,1820

	.text.windows
WriteConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteConsoleOutputCharacter,globl
	.previous
