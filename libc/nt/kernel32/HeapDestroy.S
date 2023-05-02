#include "libc/nt/codegen.h"
.imp	kernel32,__imp_HeapDestroy,HeapDestroy,0

#ifdef __x86_64__
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
#endif
