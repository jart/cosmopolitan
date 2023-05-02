#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateHardLinkW,CreateHardLinkW,0

#ifdef __x86_64__
	.text.windows
CreateHardLink:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateHardLinkW(%rip),%rax
	jmp	__sysv2nt
	.endfn	CreateHardLink,globl
	.previous
#endif
