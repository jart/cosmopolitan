.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_UnmapViewOfFileEx,UnmapViewOfFileEx,0

	.text.windows
UnmapViewOfFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnmapViewOfFileEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	UnmapViewOfFileEx,globl
	.previous
