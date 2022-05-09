.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_MapViewOfFileEx,MapViewOfFileEx,0

	.text.windows
__MapViewOfFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MapViewOfFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__MapViewOfFileEx,globl
	.previous
