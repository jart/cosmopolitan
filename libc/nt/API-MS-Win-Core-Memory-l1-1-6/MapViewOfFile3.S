#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Memory-l1-1-6,__imp_MapViewOfFile3,MapViewOfFile3

	.text.windows
	.ftrace1
MapViewOfFile3:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_MapViewOfFile3(%rip),%rax
	jmp	__sysv2nt10
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	MapViewOfFile3,globl
	.previous
