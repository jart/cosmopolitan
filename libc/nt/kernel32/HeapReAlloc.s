.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_HeapReAlloc,HeapReAlloc,0

	.text.windows
HeapReAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapReAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapReAlloc,globl
	.previous
