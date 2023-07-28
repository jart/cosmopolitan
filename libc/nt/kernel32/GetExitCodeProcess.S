#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetExitCodeProcess,GetExitCodeProcess

	.text.windows
        .ftrace1
__GetExitCodeProcess:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_GetExitCodeProcess(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__GetExitCodeProcess,globl
	.previous
