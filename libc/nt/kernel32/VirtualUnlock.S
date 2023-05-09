#include "libc/nt/codegen.h"
.imp	kernel32,__imp_VirtualUnlock,VirtualUnlock,0

	.text.windows
VirtualUnlock:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_VirtualUnlock(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	VirtualUnlock,globl
	.previous
