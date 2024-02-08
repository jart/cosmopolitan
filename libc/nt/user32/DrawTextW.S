#include "libc/nt/codegen.h"
.imp	user32,__imp_DrawTextW,DrawTextW

	.text.windows
	.ftrace1
DrawText:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_DrawTextW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DrawText,globl
	.previous
