#include "libc/nt/codegen.h"
.imp	user32,__imp_GetSystemMenu,GetSystemMenu,1955

	.text.windows
GetSystemMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemMenu(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	GetSystemMenu,globl
	.previous
