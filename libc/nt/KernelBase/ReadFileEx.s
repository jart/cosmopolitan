.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_ReadFileEx,ReadFileEx,1312

	.text.windows
ReadFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ReadFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	ReadFileEx,globl
	.previous
