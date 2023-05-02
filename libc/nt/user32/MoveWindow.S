#include "libc/nt/codegen.h"
.imp	user32,__imp_MoveWindow,MoveWindow,2176

#ifdef __x86_64__
	.text.windows
MoveWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_MoveWindow(%rip),%rax
	jmp	__sysv2nt6
	.endfn	MoveWindow,globl
	.previous
#endif
