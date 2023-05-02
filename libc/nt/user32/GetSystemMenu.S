#include "libc/nt/codegen.h"
.imp	user32,__imp_GetSystemMenu,GetSystemMenu,1955

#ifdef __x86_64__
	.text.windows
GetSystemMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemMenu(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemMenu,globl
	.previous
#endif
