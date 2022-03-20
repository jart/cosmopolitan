.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_HeapDestroy,HeapDestroy,0

	.text.windows
HeapDestroy:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_HeapDestroy(%rip)
	leave
	ret
	.endfn	HeapDestroy,globl
	.previous
