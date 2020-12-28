.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadFile,ReadFile,0

	.text.windows
ReadFile:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFile(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFile,globl
	.previous
