#include "libc/nt/codegen.h"
.imp	user32,__imp_GetDesktopWindow,GetDesktopWindow,1833

#ifdef __x86_64__
	.text.windows
GetDesktopWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetDesktopWindow(%rip)
	leave
	ret
	.endfn	GetDesktopWindow,globl
	.previous
#endif
