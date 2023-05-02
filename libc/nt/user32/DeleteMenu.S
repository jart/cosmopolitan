#include "libc/nt/codegen.h"
.imp	user32,__imp_DeleteMenu,DeleteMenu,1681

#ifdef __x86_64__
	.text.windows
DeleteMenu:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeleteMenu(%rip),%rax
	jmp	__sysv2nt
	.endfn	DeleteMenu,globl
	.previous
#endif
