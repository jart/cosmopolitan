#include "libc/nt/codegen.h"
.imp	user32,__imp_DrawTextW,DrawTextW,1731

#ifdef __x86_64__
	.text.windows
DrawText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawText,globl
	.previous
#endif
