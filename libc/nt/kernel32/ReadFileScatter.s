.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_ReadFileScatter,ReadFileScatter,0

	.text.windows
ReadFileScatter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileScatter(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFileScatter,globl
	.previous
