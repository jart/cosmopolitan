#include "libc/nt/codegen.h"
.imp	user32,__imp_ShowWindow,ShowWindow,2417

#ifdef __x86_64__
	.text.windows
ShowWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ShowWindow(%rip),%rax
	jmp	__sysv2nt
	.endfn	ShowWindow,globl
	.previous
#endif
