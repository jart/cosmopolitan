#include "libc/nt/codegen.h"
.imp	user32,__imp_GetWindowTextW,GetWindowTextW,2007

#ifdef __x86_64__
	.text.windows
GetWindowText:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetWindowTextW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetWindowText,globl
	.previous
#endif
