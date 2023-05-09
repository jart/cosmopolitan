#include "libc/nt/codegen.h"
.imp	user32,__imp_DeleteMenu,DeleteMenu,1681

	.text.windows
DeleteMenu:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DeleteMenu(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	DeleteMenu,globl
	.previous
