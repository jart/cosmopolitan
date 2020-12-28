.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteFileEx,WriteFileEx,0

	.text.windows
WriteFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFileEx,globl
	.previous
