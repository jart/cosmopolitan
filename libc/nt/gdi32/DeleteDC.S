#include "libc/nt/codegen.h"
.imp	gdi32,__imp_DeleteDC,DeleteDC,1384

#ifdef __x86_64__
	.text.windows
DeleteDC:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DeleteDC(%rip)
	leave
	ret
	.endfn	DeleteDC,globl
	.previous
#endif
