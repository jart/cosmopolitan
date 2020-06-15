.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetCompressedFileSizeA,GetCompressedFileSizeA,446

	.text.windows
GetCompressedFileSizeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCompressedFileSizeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCompressedFileSizeA,globl
	.previous
