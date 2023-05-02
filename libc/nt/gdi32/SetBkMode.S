#include "libc/nt/codegen.h"
.imp	gdi32,__imp_SetBkMode,SetBkMode,1872

#ifdef __x86_64__
	.text.windows
SetBkMode:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetBkMode(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetBkMode,globl
	.previous
#endif
