#include "libc/nt/codegen.h"
.imp	kernel32,__imp_GetConsoleScreenBufferInfoEx,GetConsoleScreenBufferInfoEx,0

	.text.windows
GetConsoleScreenBufferInfoEx:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetConsoleScreenBufferInfoEx(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetConsoleScreenBufferInfoEx,globl
	.previous
