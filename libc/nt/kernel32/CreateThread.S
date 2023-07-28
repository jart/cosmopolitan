#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateThread,CreateThread

	.text.windows
        .ftrace1
__CreateThread:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateThread(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateThread,globl
	.previous
