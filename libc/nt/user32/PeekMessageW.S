#include "libc/nt/codegen.h"
.imp	user32,__imp_PeekMessageW,PeekMessageW,2201

	.text.windows
PeekMessage:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekMessageW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	PeekMessage,globl
	.previous
