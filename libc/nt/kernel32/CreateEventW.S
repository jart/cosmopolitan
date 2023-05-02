#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateEventW,CreateEventW,0

#ifdef __x86_64__
	.text.windows
CreateEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateEventW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateEvent,globl
	.previous
#endif
