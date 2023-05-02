#include "libc/nt/codegen.h"
.imp	user32,__imp_GetShellWindow,GetShellWindow,1950

#ifdef __x86_64__
	.text.windows
GetShellWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetShellWindow(%rip)
	leave
	ret
	.endfn	GetShellWindow,globl
	.previous
#endif
