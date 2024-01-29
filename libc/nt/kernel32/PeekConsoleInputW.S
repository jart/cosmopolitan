#include "libc/nt/codegen.h"
.imp	kernel32,__imp_PeekConsoleInputW,PeekConsoleInputW

	.text.windows
	.ftrace1
PeekConsoleInput:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_PeekConsoleInputW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PeekConsoleInput,globl
	.previous
