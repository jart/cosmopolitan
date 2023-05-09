#include "libc/nt/codegen.h"
.imp	user32,__imp_AppendMenuW,AppendMenuW,1516

	.text.windows
AppendMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	AppendMenu,globl
	.previous
