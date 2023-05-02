#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateEventExW,CreateEventExW,0

#ifdef __x86_64__
	.text.windows
CreateEventEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEventEx,globl
	.previous
#endif
