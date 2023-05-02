#include "libc/nt/codegen.h"
.imp	user32,__imp_DestroyMenu,DestroyMenu,1688

#ifdef __x86_64__
	.text.windows
DestroyMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_DestroyMenu(%rip)
	leave
	ret
	.endfn	DestroyMenu,globl
	.previous
#endif
