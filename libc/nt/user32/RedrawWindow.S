#include "libc/nt/codegen.h"
.imp	user32,__imp_RedrawWindow,RedrawWindow,2246

#ifdef __x86_64__
	.text.windows
RedrawWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RedrawWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	RedrawWindow,globl
	.previous
#endif
