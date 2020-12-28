.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetCompressedFileSizeA,GetCompressedFileSizeA,0

	.text.windows
GetCompressedFileSizeA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetCompressedFileSizeA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetCompressedFileSizeA,globl
	.previous
