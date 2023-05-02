#include "libc/nt/codegen.h"
.imp	kernel32,__imp_ContinueDebugEvent,ContinueDebugEvent,0

#ifdef __x86_64__
	.text.windows
ContinueDebugEvent:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_ContinueDebugEvent(%rip),%rax
	jmp	__sysv2nt
	.endfn	ContinueDebugEvent,globl
	.previous
#endif
