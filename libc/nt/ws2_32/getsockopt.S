#include "libc/nt/codegen.h"
.imp	ws2_32,__imp_getsockopt,getsockopt,7

	.text.windows
__sys_getsockopt_nt:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_getsockopt(%rip),%rax
	jmp	__sysv2nt6
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	__sys_getsockopt_nt,globl
	.previous
