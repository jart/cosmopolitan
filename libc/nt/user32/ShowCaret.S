#include "libc/nt/codegen.h"
.imp	user32,__imp_ShowCaret,ShowCaret,2411

#ifdef __x86_64__
	.text.windows
ShowCaret:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_ShowCaret(%rip)
	leave
	ret
	.endfn	ShowCaret,globl
	.previous
#endif
