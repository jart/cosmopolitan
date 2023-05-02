#include "libc/nt/codegen.h"
.imp	user32,__imp_DrawTextExW,DrawTextExW,1730

#ifdef __x86_64__
	.text.windows
DrawTextEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DrawTextExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DrawTextEx,globl
	.previous
#endif
