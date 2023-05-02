#include "libc/nt/codegen.h"
.imp	user32,__imp_LoadImageW,LoadImageW,2105

#ifdef __x86_64__
	.text.windows
LoadImage:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadImageW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	LoadImage,globl
	.previous
#endif
