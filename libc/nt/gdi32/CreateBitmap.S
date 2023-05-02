#include "libc/nt/codegen.h"
.imp	gdi32,__imp_CreateBitmap,CreateBitmap,1056

#ifdef __x86_64__
	.text.windows
CreateBitmap:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateBitmap(%rip),%rax
	jmp	__sysv2nt6
	.endfn	CreateBitmap,globl
	.previous
#endif
