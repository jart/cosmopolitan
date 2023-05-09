#include "libc/nt/codegen.h"
.imp	user32,__imp_GetDesktopWindow,GetDesktopWindow,1833

	.text.windows
GetDesktopWindow:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	call	*__imp_GetDesktopWindow(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetDesktopWindow,globl
	.previous
