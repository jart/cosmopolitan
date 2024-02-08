#include "libc/nt/codegen.h"
.imp	user32,__imp_GetShellWindow,GetShellWindow

	.text.windows
	.ftrace1
GetShellWindow:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetShellWindow(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetShellWindow,globl
	.previous
