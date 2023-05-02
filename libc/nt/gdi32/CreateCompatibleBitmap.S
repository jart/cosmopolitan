#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateCompatibleBitmap,CreateCompatibleBitmap,1063

#ifdef __x86_64__
	.text.windows
CreateCompatibleBitmap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateCompatibleBitmap(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateCompatibleBitmap,globl
	.previous
#endif
