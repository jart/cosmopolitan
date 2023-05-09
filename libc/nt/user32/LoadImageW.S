#include "libc/nt/codegen.h"
.imp	user32,__imp_LoadImageW,LoadImageW,2105

	.text.windows
LoadImage:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadImageW(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	LoadImage,globl
	.previous
