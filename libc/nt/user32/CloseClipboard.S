#include "libc/nt/codegen.h"
.imp	user32,__imp_CloseClipboard,CloseClipboard

	.text.windows
	.ftrace1
CloseClipboard:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_CloseClipboard(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	CloseClipboard,globl
	.previous
