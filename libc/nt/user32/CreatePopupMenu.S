#include "libc/nt/codegen.h"
.imp	user32,__imp_CreatePopupMenu,CreatePopupMenu,1622

#ifdef __x86_64__
	.text.windows
CreatePopupMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_CreatePopupMenu(%rip)
	leave
	ret
	.endfn	CreatePopupMenu,globl
	.previous
#endif
