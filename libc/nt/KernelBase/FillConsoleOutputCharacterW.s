.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FillConsoleOutputCharacterW,FillConsoleOutputCharacterW,356

	.text.windows
FillConsoleOutputCharacter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FillConsoleOutputCharacterW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FillConsoleOutputCharacter,globl
	.previous
