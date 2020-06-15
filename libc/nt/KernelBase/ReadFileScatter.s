.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadFileScatter,ReadFileScatter,1313

	.text.windows
ReadFileScatter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileScatter(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFileScatter,globl
	.previous
