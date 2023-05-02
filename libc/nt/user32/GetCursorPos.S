#include "libc/nt/codegen.h"
.imp	user32,__imp_GetCursorPos,GetCursorPos,1829

#ifdef __x86_64__
	.text.windows
GetCursorPos:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetCursorPos(%rip)
	leave
	ret
	.endfn	GetCursorPos,globl
	.previous
#endif
