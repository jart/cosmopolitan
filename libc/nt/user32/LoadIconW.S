#include "libc/nt/codegen.h"
.imp	user32,__imp_LoadIconW,LoadIconW,2103

#ifdef __x86_64__
	.text.windows
LoadIcon:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadIconW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadIcon,globl
	.previous
#endif
