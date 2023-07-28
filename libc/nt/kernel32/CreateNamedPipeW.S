#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateNamedPipeW,CreateNamedPipeW

	.text.windows
        .ftrace1
__CreateNamedPipe:
        .ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CreateNamedPipeW(%rip),%rax
	jmp	__sysv2nt8
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__CreateNamedPipe,globl
	.previous
