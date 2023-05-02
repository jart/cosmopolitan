#include "libc/nt/codegen.h"
.imp	user32,__imp_GetCursor,GetCursor,1826

#ifdef __x86_64__
	.text.windows
GetCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetCursor(%rip)
	leave
	ret
	.endfn	GetCursor,globl
	.previous
#endif
