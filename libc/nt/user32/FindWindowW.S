#include "libc/nt/codegen.h"
.imp	user32,__imp_FindWindowW,FindWindowW,1784

#ifdef __x86_64__
	.text.windows
FindWindow:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindWindowW(%rip),%rax
	jmp	__sysv2nt
	.endfn	FindWindow,globl
	.previous
#endif
