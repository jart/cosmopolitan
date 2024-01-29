#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetTextJustification,SetTextJustification

	.text.windows
	.ftrace1
SetTextJustification:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_SetTextJustification(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	SetTextJustification,globl
	.previous
