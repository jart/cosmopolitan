#include "libc/nt/codegen.h"
.imp	user32,__imp_AppendMenuW,AppendMenuW,1516

#ifdef __x86_64__
	.text.windows
AppendMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_AppendMenuW(%rip),%rax
	jmp	__sysv2nt
	.endfn	AppendMenu,globl
	.previous
#endif
