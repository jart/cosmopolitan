.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_HeapFree,HeapFree,847

	.text.windows
HeapFree:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapFree(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapFree,globl
	.previous
