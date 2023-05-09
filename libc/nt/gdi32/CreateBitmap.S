#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateBitmap,CreateBitmap,1056

	.text.windows
CreateBitmap:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateBitmap(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateBitmap,globl
	.previous
