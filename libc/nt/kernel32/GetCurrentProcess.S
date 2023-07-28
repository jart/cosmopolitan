#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetCurrentProcess,GetCurrentProcess

	.text.windows
        .ftrace1
GetCurrentProcess:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	call	*__imp_GetCurrentProcess(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	GetCurrentProcess,globl
	.previous
