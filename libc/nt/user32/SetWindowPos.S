#include "libc/nt/codegen.h"
.imp	user32,__imp_SetWindowPos,SetWindowPos,2400

#ifdef __x86_64__
	.text.windows
SetWindowPos:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetWindowPos(%rip),%rax
	jmp	__sysv2nt8
	.endfn	SetWindowPos,globl
	.previous
#endif
