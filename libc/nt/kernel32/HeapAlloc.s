.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_HeapAlloc,HeapAlloc,0

	.text.windows
HeapAlloc:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapAlloc(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapAlloc,globl
	.previous
