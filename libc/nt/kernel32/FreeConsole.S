#include "libc/nt/codegen.h"
.imp	kernel32,__imp_FreeConsole,FreeConsole

	.text.windows
	.ftrace1
FreeConsole:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_FreeConsole(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	FreeConsole,globl
	.previous
