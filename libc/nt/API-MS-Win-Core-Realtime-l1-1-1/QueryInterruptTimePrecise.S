#include "libc/nt/codegen.h"
.imp	API-MS-Win-Core-Realtime-l1-1-1,__imp_QueryInterruptTimePrecise,QueryInterruptTimePrecise

	.text.windows
	.ftrace1
QueryInterruptTimePrecise:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_QueryInterruptTimePrecise(%rip)
	leave
#elif defined(__aarch64__)
	mov	x0,#0
#endif
	ret
	.endfn	QueryInterruptTimePrecise,globl
	.previous
