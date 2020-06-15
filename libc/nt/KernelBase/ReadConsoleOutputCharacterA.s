.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadConsoleOutputCharacterA,ReadConsoleOutputCharacterA,1305

	.text.windows
ReadConsoleOutputCharacterA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadConsoleOutputCharacterA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadConsoleOutputCharacterA,globl
	.previous
