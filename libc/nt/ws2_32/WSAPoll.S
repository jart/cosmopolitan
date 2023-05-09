#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_WSAPoll,WSAPoll,88

	.text.windows
WSAPoll:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WSAPoll(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	WSAPoll,globl
	.previous
