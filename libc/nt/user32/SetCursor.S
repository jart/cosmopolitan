#include "libc/nt/codegen.h"
.imp	user32,__imp_SetCursor,SetCursor,2321

#ifdef __x86_64__
	.text.windows
SetCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetCursor(%rip)
	leave
	ret
	.endfn	SetCursor,globl
	.previous
#endif
