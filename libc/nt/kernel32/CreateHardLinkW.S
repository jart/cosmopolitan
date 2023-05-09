#include "libc/nt/codegen.h"
.imp	kernel32,__imp_CreateHardLinkW,CreateHardLinkW,0

	.text.windows
CreateHardLink:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreateHardLinkW(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CreateHardLink,globl
	.previous
