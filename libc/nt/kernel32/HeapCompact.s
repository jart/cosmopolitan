.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_HeapCompact,HeapCompact,0

	.text.windows
HeapCompact:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_HeapCompact(%rip),%rax
	jmp	__sysv2nt
	.endfn	HeapCompact,globl
	.previous
