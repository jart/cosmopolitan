#include "libc/nt/codegen.h"
.imp	user32,__imp_GetMenu,GetMenu,1881

#ifdef __x86_64__
	.text.windows
GetMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_GetMenu(%rip)
	leave
	ret
	.endfn	GetMenu,globl
	.previous
#endif
