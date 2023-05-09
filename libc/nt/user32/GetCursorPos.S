#include "libc/nt/codegen.h"
.imp	user32,__imp_GetCursorPos,GetCursorPos,1829

	.text.windows
GetCursorPos:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetCursorPos(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetCursorPos,globl
	.previous
