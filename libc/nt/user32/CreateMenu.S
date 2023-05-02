#include "libc/nt/codegen.h"
.imp	user32,__imp_CreateMenu,CreateMenu,1621

#ifdef __x86_64__
	.text.windows
CreateMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_CreateMenu(%rip)
	leave
	ret
	.endfn	CreateMenu,globl
	.previous
#endif
