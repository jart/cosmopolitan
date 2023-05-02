#include "libc/nt/codegen.h"
.imp	kernel32,__imp_PeekNamedPipe,PeekNamedPipe,0

#ifdef __x86_64__
	.text.windows
PeekNamedPipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekNamedPipe(%rip),%rax
	jmp	__sysv2nt6
	.endfn	PeekNamedPipe,globl
	.previous
#endif
