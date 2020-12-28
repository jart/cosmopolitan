.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WriteFileGather,WriteFileGather,0

	.text.windows
WriteFileGather:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteFileGather(%rip),%rax
	jmp	__sysv2nt6
	.endfn	WriteFileGather,globl
	.previous
