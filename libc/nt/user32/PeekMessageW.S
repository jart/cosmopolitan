#include "libc/nt/codegen.h"
.imp	user32,__imp_PeekMessageW,PeekMessageW,2201

#ifdef __x86_64__
	.text.windows
PeekMessage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekMessageW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	PeekMessage,globl
	.previous
#endif
