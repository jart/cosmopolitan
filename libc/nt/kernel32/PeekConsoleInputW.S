#include "libc/nt/codegen.h"
.imp	kernel32,__imp_PeekConsoleInputW,PeekConsoleInputW,0

#ifdef __x86_64__
	.text.windows
PeekConsoleInput:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PeekConsoleInputW(%rip),%rax
	jmp	__sysv2nt
	.endfn	PeekConsoleInput,globl
	.previous
#endif
