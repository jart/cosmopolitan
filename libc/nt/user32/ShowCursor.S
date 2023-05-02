#include "libc/nt/codegen.h"
.imp	user32,__imp_ShowCursor,ShowCursor,2412

#ifdef __x86_64__
	.text.windows
ShowCursor:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ShowCursor(%rip)
	leave
	ret
	.endfn	ShowCursor,globl
	.previous
#endif
