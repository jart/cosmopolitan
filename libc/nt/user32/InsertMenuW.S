#include "libc/nt/codegen.h"
.imp	user32,__imp_InsertMenuW,InsertMenuW,2044

#ifdef __x86_64__
	.text.windows
InsertMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_InsertMenuW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	InsertMenu,globl
	.previous
#endif
