.include "o/libc/nt/codegen.inc"
.imp	gdi32,__imp_DeleteObject,DeleteObject,1387

	.text.windows
DeleteObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteObject(%rip)
	leave
	ret
	.endfn	DeleteObject,globl
	.previous
