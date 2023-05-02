#include "libc/nt/codegen.h"
.imp	gdi32,__imp_DeleteObject,DeleteObject,1387

#ifdef __x86_64__
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
#endif
