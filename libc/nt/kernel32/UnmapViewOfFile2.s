.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_UnmapViewOfFile2,UnmapViewOfFile2,0

	.text.windows
UnmapViewOfFile2:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnmapViewOfFile2(%rip),%rax
	jmp	__sysv2nt
	.endfn	UnmapViewOfFile2,globl
	.previous
