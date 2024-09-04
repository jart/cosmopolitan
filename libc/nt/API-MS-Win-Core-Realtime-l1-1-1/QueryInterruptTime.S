#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Realtime-l1-1-1,__imp_QueryInterruptTime,QueryInterruptTime

	.text.windows
	.ftrace1
QueryInterruptTime:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_QueryInterruptTime(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	QueryInterruptTime,globl
	.previous
