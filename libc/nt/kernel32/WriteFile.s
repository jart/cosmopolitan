.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteFile,WriteFile,0

	.text.windows
WriteFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFile,globl
	.previous
