#include "libc/nt/codegen.h"
.imp	kernel32,__imp_WriteConsoleOutputAttribute,WriteConsoleOutputAttribute,0

	.text.windows
WriteConsoleOutputAttribute:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WriteConsoleOutputAttribute(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WriteConsoleOutputAttribute,globl
	.previous
