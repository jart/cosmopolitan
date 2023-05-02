#include "libc/nt/codegen.h"
.imp	user32,__imp_SetClassLongW,SetClassLongW,2316

#ifdef __x86_64__
	.text.windows
SetClassLong:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SetClassLongW(%rip),%rax
	jmp	__sysv2nt
	.endfn	SetClassLong,globl
	.previous
#endif
